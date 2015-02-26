//===-- ReExprEvaluator.cpp ----------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/util/ReExprEvaluator.h"
#include "klee/Reorder.h"
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

void ReExprEvaluator::evalFMANONFMA(const ReorderExpr *e, vector<ref<Expr> > &res){

  if(reorderMap.find(e) != reorderMap.end()){
    res = reorderMap.find(e)->second;
    return;
  }
  
  vector<ref<Expr> > multiplicand0;
  vector<ref<Expr> > multiplicand1;
  vector<ref<Expr> > addend;

  evaluate((e->operands)[0], multiplicand0);
  evaluate((e->operands)[1], multiplicand1);
  evaluate((e->operands)[2], addend);

  for(int i = 0; i < multiplicand0.size(); i ++){
    for(int j = 0; j < multiplicand1.size(); j ++){
      for(int k = 0; k < addend.size();  k ++){
	//eval nonfma and fma
      }
    }
  }
  reorderMap[e] = res;
}


void ReExprEvaluator::evalReorderRec(const ReorderExpr *e, vector<ref<Expr> > &res, vector<ref<Expr> > &kids, int i){
  if(i == (e -> operands).size()){
    getReorderExtreme(e, kids, res);
    reorderMap[e] = res;  
  } else {
    vector<ref<Expr> > tmp;
    evaluate((e->operands)[i], tmp);
    i = i + 1;
    kids.push_back(tmp[0]);
    evalReorderRec(e, res, kids, i);
    kids.pop_back();
    if(tmp.size() == 2){
      kids.push_back(tmp[1]);
      evalReorderRec(e, res, kids, i);
      kids.pop_back();
    }
    tmp.clear();
  }
}

void ReExprEvaluator::getResMinMax(vector<ref<Expr> > &res){
  APFloat min, max;
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


void ReExprEvaluator::evalReorder(const ReorderExpr *e, vector<ref<Expr> > &res){
  if(reorderMap.find(e) != reorderMap.end()){
    res = reorderMap.find(e)->second;
    return;
  }
  vector<ref<Expr> > kids;
  vector<ref<Expr> > tmp;
  evalReorderRec(e, res, kids, 0);
  getResMinMax(res);
}

void ReExprEvaluator::getReorderExtreme(const ReorderExpr *e, vector<ref<Expr> > &kids,vector<ref<Expr> > &res){
  APFloat max, min;
  vector<APFloat> ops;
  vector<APFloat> opl;
  vector<APFloat> opr;
  Reorder ro(FE_TONEAREST);

  if(e->cat == Expr::RE_FMA){
    for(int i = 0; i < kids.size(); i = i + 2){
      if(ConstantExpr *CE= dyn_cast<ConstantExpr>(kids[i])){
	opl.push_back(CE->getAPFValue());
      }
      if(ConstantExpr *CE= dyn_cast<ConstantExpr>(kids[i+1])){
	opr.push_back(CE->getAPFValue());
      }
    }
  }else{
    for(int i = 0; i < kids.size(); i ++){
      if(ConstantExpr *CE = dyn_cast<ConstantExpr>(kids[i])){
	ops.push_back(CE->getAPFValue());
      }
    }
  }

  switch(e->cat){
  case Expr::RE_FMA:{
    max = ro.getFMAMax(opl, opr);
    min = ro.getFMAMin(opl, opr);
    break;
  }
  case Expr::RE_Plus:{
    max = ro.getPlusMax(ops);
    min = ro.getPlusMin(ops);
    break;
  }
  case Expr::RE_Mult:{
    max = ro.getMultMax(ops);
    min = ro.getMultMin(ops);
    break;
  }
  default:
    assert(0 && "unsupported reorderable expression");
  }

  ref<ConstantExpr> ceMin = ConstantExpr::alloc(min);
  ref<ConstantExpr> ceMax = ConstantExpr::alloc(max);
  res.push_back(ceMin);
  res.push_back(ceMax);
  return;
}

void ReExprEvaluator::evalFComp(const ref<Expr> &e, vector<ref<Expr> > &res){
  vector<ref<Expr> > kidRes;
  ref<ConstantExpr> minDist;
  ref<ConstantExpr> minValue;
  ref<Expr> tmp[2];
  if(ConstantExpr *CER = dyn_cast<ConstantExpr>(e->getKid(1))){
    tmp[1] = e->getKid(1);
    evaluate(e->getKid(0), kidRes);
    for(int i = 0; i < kidRes.size(); i ++){
      if(ConstantExpr *CEL = dyn_cast<ConstantExpr>(kidRes[i])){
	ref<ConstantExpr> value = CER->FSub(CEL);
	ref<ConstantExpr> dist = CER->FAbs(CEL);
	if(minValue.get()){
	  if(isMinEqMax){
	    llvm::APFloat apfmin = minValue -> getAPFValue();
	    llvm::APFloat apfv = value -> getAPFValue();
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
	tmp[0] = kidRes[i];
	res.push_back(e->rebuild(tmp));
      }else{
	assert(0 && "encounter non-constantExpr after evaluate");
      }
    }
  }
  epsilon = minValue;
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


