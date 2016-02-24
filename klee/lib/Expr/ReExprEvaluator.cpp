//===-- ReExprEvaluator.cpp ----------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/util/ReExprEvaluator.h"
#include "llvm/ADT/APFloat.h"

#include <iostream>
#include <iomanip> 
#include <cstdlib> 
#include <math.h>

using namespace klee;
using namespace llvm;
using namespace std;

/***/

void ReExprEvaluator::evalRead(const ReadExpr *e, vector<ref<Expr> > &res){
  if(ConstantExpr *CE = dyn_cast<ConstantExpr>(e->index)){
    evalUpdate(e->updates, CE->getZExtValue(), res);
  }else{
    assert(0 && "encounter non constant index update");
  }
}

void ReExprEvaluator::evalUpdate(const UpdateList &ul,
				 unsigned index, 
				 vector<ref<Expr> > &res) {
  for (const UpdateNode *un=ul.head; un; un=un->next) {    
    if (ConstantExpr *CE = dyn_cast<ConstantExpr>(un->index)) {
      if (CE->getZExtValue() == index){
	evaluate(un->value, res);
	return;
      } else {
	// update index is unknown, so may or may not be index, we
	// cannot guarantee value. we can rewrite to read at this
	// version though (mostly for debugging).
	assert(0 && "update index is unknown");
      }
    }
  }
  //  if (ul.root->isConstantArray() && index < ul.root->size)
  //    return Action::changeTo(ul.root->constantValues[index]);

  getInitialValue(*ul.root, index, res);
}


void ReExprEvaluator::evalMultRec(const vector<ref<Expr> > &ops, vector<ref<Expr> > &kids, 
				  int i, vector<MultRes> &res){
  if(i == ops.size()){
    vector<MultRes> localRes;
    Reorder ro(FE_TONEAREST);
    ro.getMultBounds(kids, localRes);
    if(res.size() == 0){
      res.push_back(localRes[0]);
      res.push_back(localRes[1]);
    } else {
      ref<ConstantExpr> localMin = dyn_cast<ConstantExpr>(localRes[0].res);
      ref<ConstantExpr> globalMin = dyn_cast<ConstantExpr>(res[0].res);
      ref<ConstantExpr> localMax = dyn_cast<ConstantExpr>(localRes[1].res);
      ref<ConstantExpr> globalMax = dyn_cast<ConstantExpr>(res[1].res);
      
      if((localMin->FOlt(globalMin))->isTrue()){
	res[0] = localRes[0];
      }
      if((localMax->FOgt(globalMax))->isTrue()){
	res[1] = localRes[1];
      }
    }
  } else {
    vector<ref<Expr> > tmp;
    evaluate(ops[i], tmp);
    i = i + 1;
    kids.push_back(tmp[0]);
    evalMultRec(ops, kids, i, res);
    kids.pop_back();
    if(tmp.size() == 2){
      kids.push_back(tmp[1]);
      evalMultRec(ops, kids, i, res);
      kids.pop_back();
    }
    tmp.clear();
  }
}

void ReExprEvaluator::constructMult(const ref<Expr> &src, vector<ref<Expr> > &ops){
  if(src -> getKind() == Expr::FMul){
    BinaryExpr *be = cast<BinaryExpr>(src);
    constructMult(be->left, ops);
    constructMult(be->right, ops);
  }else{
    ops.push_back(src);
  }
}

void ReExprEvaluator::evalReOps(const ref<Expr> &e, vector<pair<ref<Expr>, ref<Expr> > > &minVec,
				vector<pair<ref<Expr>, ref<Expr> > > &maxVec){
  if(e -> getKind() == Expr::FMul){ //detect a*b*c
    vector<ref<Expr > > ops, kids;
    vector<MultRes> res;
    constructMult(e, ops);

    evalMultRec(ops, kids, 0, res);
    pair<ref<Expr>, ref<Expr> > minPair(res[0].op1, res[0].op2);
    pair<ref<Expr>, ref<Expr> > maxPair(res[1].op1, res[1].op2); 
    minVec.push_back(minPair);
    maxVec.push_back(maxPair);
  } else {
    APFloat cons(1.0f);
    ref<Expr> cons1 = ConstantExpr::alloc(cons);
    vector<ref<Expr> > tmp;
    evaluate(e, tmp);
    if(tmp.size() == 1){
      pair<ref<Expr>, ref<Expr> > minPair(tmp[0], cons1);//a*1
      minVec.push_back(minPair);
      maxVec.push_back(minPair);
    }else{
      pair<ref<Expr>, ref<Expr> > minPair(tmp[0], cons1);//a*1
      pair<ref<Expr>, ref<Expr> > maxPair(tmp[1], cons1); 
      minVec.push_back(minPair);
      maxVec.push_back(maxPair);
    }

  }
}

void ReExprEvaluator::evalReorder(const ReorderExpr *e, vector<ref<Expr> > &res){
  if(reorderMap.find(e) != reorderMap.end()){
    res = reorderMap.find(e)->second;
    return;
  }

  vector<pair<ref<Expr>, ref<Expr> > > minVec;
  vector<pair<ref<Expr>, ref<Expr> > > maxVec;
  

  for(int i = 0; i < (e -> operands).size(); i ++){
    
    evalReOps((e -> operands)[i], minVec, maxVec);
  }

  Reorder ro(FE_TONEAREST);
  vector<ref<Expr> > minRes, maxRes;
  ro.getDotBounds(minVec, minRes);
  ro.getDotBounds(maxVec, maxRes);

  res.push_back(minRes[0]);
  res.push_back(maxRes[1]);

  reorderMap[e] = res;  
  return;
}

void ReExprEvaluator::evalFComp(const ref<Expr> &e, vector<ref<Expr> > &res){
  vector<ref<Expr> > kidRes;
  ref<ConstantExpr> minDist;
  ref<ConstantExpr> minValue;
  ref<Expr> tmp[2];
  if(ConstantExpr *CER = dyn_cast<ConstantExpr>(e->getKid(0))){
    tmp[0] = e->getKid(0);
    evaluate(e->getKid(1), kidRes);
    for(int i = 0; i < kidRes.size(); i ++){
      cout << "fcomp" << endl;
      if(ConstantExpr *CEL = dyn_cast<ConstantExpr>(kidRes[i])){
	
	ref<ConstantExpr> value = CER->FSub(CEL);
	ref<ConstantExpr> dist = CER->FAbs(CEL);

	if(minValue.get()){
	  if(isMinEqMax){
	    llvm::APFloat apfmin = minValue -> getAPFValue();
	    llvm::APFloat apfv = value -> getAPFValue();
	    cout << apfv.convertToFloat() << endl;
	    
            if(value -> getWidth() == Expr::Int32){
	      if(apfmin.convertToFloat() != apfv.convertToFloat()){
		isMinEqMax = false;
	      }
	    }else if(value -> getWidth() == Expr::Int64){
	      if(apfmin.convertToDouble() != apfv.convertToDouble()){
		isMinEqMax = false;
	      }
	    }
	  }

	  if((minDist -> FOgt(dist)) -> isTrue()){
	    minValue = value;
	    minDist = dist;
	  }

	}else{
	  minValue = value;
	  minDist = dist;
	}
	tmp[1] = kidRes[i];
	res.push_back(e->rebuild(tmp));
      }else{
	assert(0 && "encounter non-constantExpr after evaluate");
      }
    }
  }
  epsilon = minValue;
} 

void ReExprEvaluator::getResMinMax(vector<ref<Expr> > &res){
  APFloat min(APFloat::IEEEsingle), max(APFloat::IEEEsingle);
  if(ConstantExpr *CE = dyn_cast<ConstantExpr>(res[0])){
    min = CE->getAPFValue();
    max = CE->getAPFValue();
  }
  for(int i = 1; i< res.size(); i ++){
    if(ConstantExpr *CE = dyn_cast<ConstantExpr>(res[i])){
      if(min.compare(CE->getAPFValue()) == APFloat::cmpGreaterThan){
	min = CE->getAPFValue();
      }else if(max.compare(CE->getAPFValue()) == APFloat::cmpLessThan){
	max = CE->getAPFValue();
      }      
    }
  }
  res.clear();
  ref<ConstantExpr> ceMin = ConstantExpr::alloc(min);
  ref<ConstantExpr> ceMax = ConstantExpr::alloc(max);
  res.push_back(ceMin);
  res.push_back(ceMax);
}

void ReExprEvaluator::evaluate(const ref<Expr> &e, vector<ref<Expr> > &res){
  if(isa<ConstantExpr>(e)){
    res.push_back(e);
    return;
  } else {
    switch(e->getKind()){
    case Expr::Reorder:{
      ReorderExpr *re = dyn_cast<ReorderExpr>(e);
      evalReorder(re, res);
      break;
    }

    case Expr::Read:{
      evalRead(dyn_cast<ReadExpr>(e), res);
      break;
    }

    case Expr::FOlt:
    case Expr::FOle:
    case Expr::FUeq:{
      evalFComp(e, res);
      break;
    }

    case Expr::InvalidKind:{
      assert(0 && "evaluate invalid expr");
      break;
    }

    default: {
      vector<vector<ref<Expr> > > kids;
      unsigned count = e->getNumKids();
      kids.resize(count);
      assert(count < 3); //Fix me: need to handle select

      for(unsigned i = 0; i < count; i ++){
	ref<Expr> kid = e->getKid(i);
        evaluate(kid, kids[i]);
      }      
      if(count == 1){
	for(unsigned i = 0; i < kids[0].size(); i ++){
	  res.push_back(kids[0][i]);
	}
      } else { // for count == 2
	ref<Expr> tmp[2];
        for(unsigned i = 0; i < kids[0].size(); i ++){	    
	  for(unsigned j = 0; j < kids[1].size(); j ++){
	      tmp[0] = kids[0][i];
	      tmp[1] = kids[1][j];
	      res.push_back(e->rebuild(tmp));
	  }
	}
      }
      getResMinMax(res);
    }      
    }
  }
}

ReExprEvaluator::EvalState ReExprEvaluator::isAssignmentStable(const ref<Expr> &e, ref<Expr> &eps){
  bool trueRe = false, falseRe = false;
  vector<ref<Expr> > res;

  evaluate(e,res);

  vector<ref<Expr> >::iterator ite = res.begin();
  for(; ite != res.end(); ite++){
    ref<Expr> v = *ite;
    if(ConstantExpr *CE = dyn_cast<ConstantExpr>(v)){
      if(CE->isTrue()){
	trueRe = true;
      }else if(CE->isFalse()){
	falseRe = true;
      }
    }else{
      assert(0 && "encounter non-constant result in ReExprEvaluator");
    }
  }
  eps = epsilon;

  if(trueRe && falseRe){
    return Success;
  }else if(isMinEqMax){
    return MinEqualMax;
  }
  return Epsilon;
}


