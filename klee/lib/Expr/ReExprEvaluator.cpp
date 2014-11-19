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
using namespace std;

/***/

void ReExprEvaluator::evalRead(const ReadExpr *e, vector<ref<Expr> > &res){
  if((e->index)->getKind() == Expr::InvalidKind){
    evalUpdate(e->updates, res);
    return;
  }else{
    vector<ref<Expr> > iRes;
    evaluate(e->index, iRes);
    if (ConstantExpr *CE = dyn_cast<ConstantExpr>(iRes[0])) {
      evalUpdate(e->updates, CE->getZExtValue(), res);
      return;
    } else {
      assert(0 && "encounter non constant index update");
    }
  }
}

void ReExprEvaluator::evalUpdate(const UpdateList &ul,
				   unsigned index, 
				   vector<ref<Expr> > &res) {
for (const UpdateNode *un=ul.head; un; un=un->next) {
  vector<ref<Expr> > iRes;
  evaluate(un->index, iRes);
    
  if (ConstantExpr *CE = dyn_cast<ConstantExpr>(iRes[0])) {
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

void ReExprEvaluator::evalUpdate(const UpdateList &ul, vector<ref<Expr> > &res){
  const UpdateNode *un=ul.head;
  if(un){
evaluate(un->value, res);
    return;
  }

  getInitialValue(*ul.root, 0, res); //We do not use index here, 0 is a fake index
  return;
}

void ReExprEvaluator::evalReorder(const ReorderExpr *e, vector<ref<Expr> > &res){
  //in this function we call the min/max method 
  //Fix me: need to add type support, currently we assume float
  //Fix me: need to add round mode support
  float max = 0.0f, min = 0.0f;
  vector<float> ops;
  vector<float> opl;
  vector<float> opr;
  int len = (e->operands).size();

  Reorder ro(FE_TONEAREST);

  vector<ref<Expr> > kids;
  vector<ref<Expr> > tmp;

  srand(time(0));
  int random;

  for(int i = 0; i < len; i ++){
    evaluate((e->operands)[i], tmp);
    kids.push_back(tmp[0]);
    tmp.clear();
  }
  
  if(e->cat == Expr::RE_FMA){
    for(int i = 0; i < len; i = i + 2){
      if(ConstantExpr *CE = dyn_cast<ConstantExpr>(kids[i])){        
	llvm::APFloat v = CE->getAPFValue();
	random = rand()%100+1;
	float x = v.convertToFloat();
	/*for(int j = 0; j < random; j ++){
	  x = ::nextafterf(x, 1.0);
	  }*/
	opl.push_back(x);
      }else{
	assert(0 && "encounter non-constant in Reorder Rebuild");
      }

      if(ConstantExpr *CE = dyn_cast<ConstantExpr>(kids[i+1])){
	llvm::APFloat v = CE->getAPFValue();
        random = rand()%100+1;
	float x = v.convertToFloat();
	/*for(int j = 0; j < random; j ++){
	  x = ::nextafterf(x, 1.0);
	  }*/
	opr.push_back(x);
      }else{
	assert(0 && "encounter non-constant in Reorder Rebuild");
      }      
    }
  } else {
    for(int i = 0; i < len; i ++){
      if(ConstantExpr *CE = dyn_cast<ConstantExpr>(kids[i])){
	llvm::APFloat v = CE->getAPFValue();
	ops.push_back(v.convertToFloat());
      }else{
	assert(0 && "encounter non-constant in Reorder Rebuild");
      }
    }
  }

  switch(e->cat){
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

  case Expr::RE_FMA:{
    max = ro.getFMAMax(opl, opr);
    min = ro.getFMAMin(opl, opr);
    break;
  }
  default:
    assert(0 && "unsupported reorderable expression");
  }
  std::cout << "min: " << std::setprecision(9) << min << std::endl;
  std::cout << "max: " << std::setprecision(9) << max << std::endl;
  llvm::APFloat apMax(max), apMin(min);
  res.push_back(ConstantExpr::alloc(apMin));
  res.push_back(ConstantExpr::alloc(apMax));
  return;
}

void ReExprEvaluator::evalFOlt(const FOltExpr *e, vector<ref<Expr> > &res){
  //Fix me:: handle e-> left is constant
  vector<ref<Expr> > kidEvalRes;
  ref<ConstantExpr> minDist;
  ref<ConstantExpr> minValue;
  if(ConstantExpr *CER = dyn_cast<ConstantExpr>(e->getKid(0))){
    evaluate(e->getKid(1), kidEvalRes);
    ref<Expr> tmp[2];
    tmp[0] = e->getKid(0);
    for(int i = 0; i < kidEvalRes.size(); i ++){
      if(ConstantExpr *CEL = dyn_cast<ConstantExpr>(kidEvalRes[i])){
	std::string test;
	CEL->toString(test, 10, 1);
	std::cout << "extreme value: " << test << std::endl;
	ref<ConstantExpr> value = CER->FSub(CEL);
	ref<ConstantExpr> dist = CER->FAbs(CEL);
	if(minValue.get()){
	  if((minDist -> FOgt(dist)) -> isTrue()){
	    minValue = value;
	    minDist = dist;
	  }
	}else{
	  minValue = value;
	  minDist = dist;
	}
      }else{
	assert(0 && "encounter non-constantExpr after evaluate");
      }
      tmp[1] = kidEvalRes[i];
      res.push_back(e->rebuild(tmp));
    }
  }else if(ConstantExpr *CE = dyn_cast<ConstantExpr>(e->getKid(1))){
    evaluate(e->getKid(0), kidEvalRes);
    ref<Expr> tmp[2];
    tmp[1] = e->getKid(1);
    for(int i = 0; i < kidEvalRes.size(); i ++){
      if(ConstantExpr *CEL = dyn_cast<ConstantExpr>(kidEvalRes[i])){
	ref<ConstantExpr> value = CE->FSub(CEL);
	ref<ConstantExpr> dist = CE->FAbs(CEL);
        if(minValue.get()){
	  if((minDist -> FOgt(dist)) -> isTrue()){
	    minValue = value;
	    minDist = dist;
	  }
	}else{
	  minValue = value;
	  minDist = dist;
	}
      }else{
	assert(0 && "encounter non-constantExpr after evaluate");
      }
      tmp[0] = kidEvalRes[i];
      res.push_back(e->rebuild(tmp));
    }
  }else{
    assert(0 && "we need one side of evalFOlt is constant");
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
      evalReorder(dyn_cast<ReorderExpr>(e), res);
      break;
    }

    case Expr::Read:{
      evalRead(dyn_cast<ReadExpr>(e), res);
      break;
    }

    case Expr::FOlt:{
      evalFOlt(dyn_cast<FOltExpr>(e), res);
      break;
    }

    case Expr::InvalidKind:{
      assert(0 && "evaluate invalid expr");
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
	  tmp[0] = kids[0][i];
	  for(unsigned j = 0; j < kids[1].size(); j ++){
	    tmp[1] = kids[1][j];
	    res.push_back(e->rebuild(tmp));
	  }
	}
      }
      return;
    }      
    }
  }
}

bool ReExprEvaluator::isAssignmentStable(const ref<Expr> &e, ref<Expr> &eps){
  bool trueRe = false, falseRe = false;
  vector<ref<Expr> > res;
  evaluate(e,res);
  vector<ref<Expr> >::iterator ite = res.begin();

  for(; ite != res.end(); ite++){
    if(ConstantExpr *CE = dyn_cast<ConstantExpr>(*ite)){
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
  return trueRe && falseRe;
}
