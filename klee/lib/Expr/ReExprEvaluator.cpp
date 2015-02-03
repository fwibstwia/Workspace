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

void ReExprEvaluator::evalRead(const ReadExpr *e, vector<ReExprRes> &res){
  if(ConstantExpr *CE = dyn_cast<ConstantExpr>(e->index)){
    evalUpdate(e->updates, CE->getZExtValue(), res);
  }else{
    assert(0 && "encounter non constant index update");
  }
}

void ReExprEvaluator::evalUpdate(const UpdateList &ul,
				   unsigned index, 
				   vector<ReExprRes> &res) {
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

void ReExprEvaluator::evalReorderFMANONFMA(const ReorderExpr *e, vector<ReExprRes> &res){

  if(reorderMap.find(e) != reorderMap.end()){
    res = reorderMap.find(e)->second;
    return;
  }
  
  vector<ReExprRes> multiplicand0;
  vector<ReExprRes> multiplicand1;
  vector<ReExprRes> addend;

  evaluate((e->operands)[0], multiplicand0);
  evaluate((e->operands)[1], multiplicand1);
  evaluate((e->operands)[2], addend);

  for(int i = 0; i < multiplicand0.size(); i ++){
    for(int j = 0; j < multiplicand1.size(); j ++){
      for(int k = 0; k < addend.size();  k ++){
	if(!multiplicand0[i].isConflict(multiplicand1[j])   
	   && !multiplicand0[i].isConflict(addend[k])
	   && !multiplicand1[j].isConflict(addend[k])){
	  if(multiplicand0[i].isFMAExpIn() || multiplicand1[j].isFMAExpIn() ||
	     addend[k].isFMAExpIn()){
	    evalFMAExp(multiplicand0[i].getResVal(), 
		       multiplicand1[j].getResVal(), addend[k].getResVal(), res);
	  }else if(multiplicand0[i].isNonFMAExpIn() || multiplicand1[j].isNonFMAExpIn() ||
		   addend[k].isNonFMAExpIn()){
	    evalNonFMAExp(multiplicand0[i].getResVal(), 
			  multiplicand1[j].getResVal(), addend[k].getResVal(), res);
	  }else{
	    evalFMAExp(multiplicand0[i].getResVal(), 
		       multiplicand1[j].getResVal(), addend[k].getResVal(), res);
	    evalNonFMAExp(multiplicand0[i].getResVal(), 
			  multiplicand1[j].getResVal(), addend[k].getResVal(), res);
	  }
	}
      }
    }
  }
  reorderMap[e] = res;
}

void ReExprEvaluator::evalNonFMAExp(const ref<Expr> mult0, const ref<Expr> mult1,
				    const ref<Expr> addend, vector<ReExprRes> &res){
  ConstantExpr *mult0CE = dyn_cast<ConstantExpr>(mult0);
  ConstantExpr *mult1CE = dyn_cast<ConstantExpr>(mult1);
  ConstantExpr *addendCE = dyn_cast<ConstantExpr>(addend);
  if(mult0CE->getWidth() == Expr::Int32){
    float mult0_v = (mult0CE->getAPFValue()).convertToFloat();
    float mult1_v = (mult1CE->getAPFValue()).convertToFloat();
    float addend_v = (addendCE->getAPFValue()).convertToFloat();
    vector<float> ops;
    ops.push_back(mult0_v*mult1_v);
    ops.push_back(addend_v);
    Reorder<float> ro(FE_TONEAREST);
    float nonfma = ro.getPlusMin(ops);
    llvm::APFloat apNonfma(nonfma);
    ref<Expr> nonfmaExpr = ConstantExpr::alloc(apNonfma);
    set<int64_t> reorders;
    set<int64_t> reorderCompls;
    reorders.insert((int64_t)0);
    reorderCompls.insert((int64_t)1);
    res.push_back(ReExprRes(reorderCompls, reorders, nonfmaExpr)); 

  }else if(mult0CE->getWidth() == Expr::Int64){
    double mult0_v = (mult0CE->getAPFValue()).convertToDouble();
    double mult1_v = (mult1CE->getAPFValue()).convertToDouble();
    double addend_v = (addendCE->getAPFValue()).convertToDouble();
    vector<double> ops;
    ops.push_back(mult0_v*mult1_v);
    ops.push_back(addend_v);
    Reorder<double> ro(FE_TONEAREST);
    double nonfma = ro.getPlusMin(ops);
    llvm::APFloat apNonfma(nonfma);
    ref<Expr> nonfmaExpr = ConstantExpr::alloc(apNonfma);
    set<int64_t> reorders;
    set<int64_t> reorderCompls;
    reorders.insert((int64_t)0);
    reorderCompls.insert((int64_t)1);
    res.push_back(ReExprRes(reorderCompls, reorders, nonfmaExpr)); 
  }
}

void ReExprEvaluator::evalFMAExp(const ref<Expr> mult0, const ref<Expr> mult1, 
				 const ref<Expr> addend, vector<ReExprRes> &res){

  ConstantExpr *mult0CE = dyn_cast<ConstantExpr>(mult0);
  ConstantExpr *mult1CE = dyn_cast<ConstantExpr>(mult1);
  ConstantExpr *addendCE = dyn_cast<ConstantExpr>(addend);
  if(mult0CE->getWidth() == Expr::Int32){
    float mult0_v = (mult0CE->getAPFValue()).convertToFloat();
    float mult1_v = (mult1CE->getAPFValue()).convertToFloat();
    float addend_v = (addendCE->getAPFValue()).convertToFloat();
    Reorder<float> ro(FE_TONEAREST);
    float fma = ro.getFMAExp(mult0_v, mult1_v, addend_v);
    llvm::APFloat apFma(fma);
    ref<Expr> fmaExpr = ConstantExpr::alloc(apFma);
    set<int64_t> reorders;
    set<int64_t> reorderCompls;
    reorders.insert((int64_t)0);
    reorderCompls.insert((int64_t)1);
    res.push_back(ReExprRes(reorders, reorderCompls, fmaExpr));

  }else if(mult0CE->getWidth() == Expr::Int64){
    double mult0_v = (mult0CE->getAPFValue()).convertToDouble();
    double mult1_v = (mult1CE->getAPFValue()).convertToDouble();
    double addend_v = (addendCE->getAPFValue()).convertToDouble();
    Reorder<double> ro(FE_TONEAREST);
    double fma = ro.getFMAExp(mult0_v, mult1_v, addend_v);
    llvm::APFloat apFma(fma);
    ref<Expr> fmaExpr = ConstantExpr::alloc(apFma);
    set<int64_t> reorders;
    set<int64_t> reorderCompls;
    reorders.insert((int64_t)0);
    reorderCompls.insert((int64_t)1);
    res.push_back(ReExprRes(reorders, reorderCompls, fmaExpr));
  }
}

void ReExprEvaluator::evalReorder(const ReorderExpr *e, vector<ReExprRes> &res){
  //in this function we call the min/max method 
  //Fix me: need to add round mode support
  if(reorderMap.find(e) != reorderMap.end()){
    res = reorderMap.find(e)->second;
    return;
  }

  int len = (e->operands).size();
  vector<ReExprRes> kids;
  vector<ReExprRes> tmp;

  for(int i = 0; i < len; i ++){
    evaluate((e->operands)[i], tmp);
    kids.push_back(tmp[0]);
    tmp.clear();
  }

  //detect the type of the operands
 if(ConstantExpr *CE = dyn_cast<ConstantExpr>(kids[0].getResVal())){    
   if(CE -> getWidth() == Expr::Int32){
     getReorderExtreme<float>(e, kids, res);
   }else if(CE -> getWidth() == Expr::Int64){
     getReorderExtreme<double>(e, kids, res);
   }
  reorderMap[e] = res;
 }else{
   assert(0 && "encounter non-constant in Reorder Rebuild");
 }  
}

template <typename T>
void ReExprEvaluator::getReorderExtreme(const ReorderExpr *e, vector<ReExprRes> &kids,vector<ReExprRes> &res){
  T max = 0.0f, min = 0.0f, fmaMax = 0.0f, fmaMin = 0.0f;
  vector<T> ops;
  vector<T> opl;
  vector<T> opr;
  Reorder<T> ro(FE_TONEAREST);

  if(e->cat == Expr::RE_FMA){
    for(int i = 0; i < kids.size(); i = i + 2){
      T x,y;
      if(ConstantExpr *CE = dyn_cast<ConstantExpr>(kids[i].getResVal())){        
	llvm::APFloat v = CE->getAPFValue();
	if(sizeof(T) == 4){
	  x = v.convertToFloat();
	}else if(sizeof(T) == 8){
	  x = v.convertToDouble();
	}
	opl.push_back(x);
      }else{
	assert(0 && "encounter non-constant in Reorder Rebuild");
      }

      if(ConstantExpr *CE = dyn_cast<ConstantExpr>(kids[i+1].getResVal())){
	llvm::APFloat v = CE->getAPFValue();
	if(sizeof(T) == 4){
	  y = v.convertToFloat();
	}else if(sizeof(T) == 8){
	  y = v.convertToDouble();
	}
	opr.push_back(y);
      }else{
	assert(0 && "encounter non-constant in Reorder Rebuild");
      }
      
      ops.push_back(x*y);     
    }
  }else {
    for(int i = 0; i < kids.size(); i ++){
      if(ConstantExpr *CE = dyn_cast<ConstantExpr>(kids[i].getResVal())){
	llvm::APFloat v = CE->getAPFValue();
        if(sizeof(T) == 4){
	  ops.push_back(v.convertToFloat());
	}else if(sizeof(T) == 8){
	  ops.push_back(v.convertToDouble());
	}
      }else{
	assert(0 && "encounter non-constant in Reorder Rebuild");
      }
    }
  }

  switch(e->cat){
  case Expr::RE_FMA:{
    vector<ref<Expr> > extremes;
    fmaMax = ro.getFMAMax(opl, opr);
    fmaMin = ro.getFMAMin(opl, opr);
    max = ro.getPlusMax(ops);
    min = ro.getPlusMin(ops);

    llvm::APFloat apFmaMax(fmaMax), apFmaMin(fmaMin),
      apMax(max), apMin(min);
    extremes.push_back(ConstantExpr::alloc(apFmaMin));
    extremes.push_back(ConstantExpr::alloc(apFmaMax));
    extremes.push_back(ConstantExpr::alloc(apMin));
    extremes.push_back(ConstantExpr::alloc(apMax));   
    
    for(int i = 0; i < extremes.size(); i ++){
      set<int64_t> reorders;
      set<int64_t> reorderCompls;
      reorders.insert((int64_t)&extremes[i]);
      for(int j = 0; j < extremes.size(); j ++){
	if(j != i){
	  reorderCompls.insert((int64_t)&extremes[j]);
	}
      }
      res.push_back(ReExprRes(reorders, reorderCompls, extremes[i]));
    }
    return;
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

  set<int64_t> reorders;
  set<int64_t> reorderCompls;

  llvm::APFloat apMin(min), apMax(max);
  ref<Expr> minExpr = ConstantExpr::alloc(apMin);
  ref<Expr> maxExpr = ConstantExpr::alloc(apMax);
  
  reorders.insert((int64_t)&minExpr);
  reorderCompls.insert((int64_t)&maxExpr);
  res.push_back(ReExprRes(reorders, reorderCompls, minExpr));
  res.push_back(ReExprRes(reorderCompls, reorders, maxExpr));  


  return;
}

void ReExprEvaluator::evalFUeq(const FUeqExpr *e, vector<ReExprRes> &res){
  //Fix me:: handle e-> left is constant
  vector<ReExprRes> kidRes;
  ref<ConstantExpr> minDist;
  ref<ConstantExpr> minValue;
  if(ConstantExpr *CER = dyn_cast<ConstantExpr>(e->getKid(1))){
    ref<Expr> tmp[2];
    tmp[1] = e->getKid(1);
    evaluate(e->getKid(0), kidRes);
    for(int i = 0; i < kidRes.size(); i ++){
      if(ConstantExpr *CEL = dyn_cast<ConstantExpr>(kidRes[i].getResVal())){
	//std::string test;
	//CEL->toString(test, 10, 1);
	//std::cout << "extreme value: " << test << std::endl;

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
      }else{
	assert(0 && "encounter non-constantExpr after evaluate");
      }
      ReExprRes re(kidRes[i]);
      tmp[0] = kidRes[i].getResVal();
      re.setResVal(e->rebuild(tmp));
      res.push_back(re);
    }
  }
  epsilon = minValue;
} 

void ReExprEvaluator::evalFOle(const FOleExpr *e, vector<ReExprRes> &res){
  //Fix me:: handle e-> left is constant
  vector<ReExprRes> kidRes;
  ref<ConstantExpr> minDist;
  ref<ConstantExpr> minValue;
  if(ConstantExpr *CER = dyn_cast<ConstantExpr>(e->getKid(1))){
    ref<Expr> tmp[2];
    tmp[1] = e->getKid(1);
    evaluate(e->getKid(0), kidRes);
    for(int i = 0; i < kidRes.size(); i ++){
      if(ConstantExpr *CEL = dyn_cast<ConstantExpr>(kidRes[i].getResVal())){
	//std::string test;
	//CEL->toString(test, 10, 1);
	//std::cout << "extreme value: " << test << std::endl;

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
      }else{
	assert(0 && "encounter non-constantExpr after evaluate");
      }
      ReExprRes re(kidRes[i]);
      tmp[0] = kidRes[i].getResVal();
      re.setResVal(e->rebuild(tmp));
      res.push_back(re);
    }
  }
  epsilon = minValue;
} 

void ReExprEvaluator::evalFOlt(const FOltExpr *e, vector<ReExprRes> &res){
  //Fix me:: handle e-> left is constant
  vector<ReExprRes> kidRes;
  ref<ConstantExpr> minDist;
  ref<ConstantExpr> minValue;
  if(ConstantExpr *CER = dyn_cast<ConstantExpr>(e->getKid(0))){
    ref<Expr> tmp[2];
    tmp[0] = e->getKid(0);
    evaluate(e->getKid(1), kidRes);
    for(int i = 0; i < kidRes.size(); i ++){
      if(ConstantExpr *CEL = dyn_cast<ConstantExpr>(kidRes[i].getResVal())){
	
	//std::string test;
	//CEL->toString(test, 10, 1);
	//std::cout << "extreme value: " << test << std::endl;

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
      }else{
	assert(0 && "encounter non-constantExpr after evaluate");
      }
      ReExprRes re(kidRes[i]);
      tmp[1] = kidRes[i].getResVal();
      re.setResVal(e->rebuild(tmp));
      res.push_back(re);
    }
  }else if(ConstantExpr *CER = dyn_cast<ConstantExpr>(e->getKid(1))){
    ref<Expr> tmp[2];
    tmp[1] = e->getKid(1);
    evaluate(e->getKid(0), kidRes);
    for(int i = 0; i < kidRes.size(); i ++){
      if(ConstantExpr *CEL = dyn_cast<ConstantExpr>(kidRes[i].getResVal())){
	//std::string test;
	//CEL->toString(test, 10, 1);
	//std::cout << "extreme value: " << test << std::endl;

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
      }else{
	assert(0 && "encounter non-constantExpr after evaluate");
      }
      ReExprRes re(kidRes[i]);
      tmp[0] = kidRes[i].getResVal();
      re.setResVal(e->rebuild(tmp));
      res.push_back(re);
    }
  }
  epsilon = minValue;
} 

void ReExprEvaluator::evaluate(const ref<Expr> &e, vector<ReExprRes> &res){
  if(isa<ConstantExpr>(e)){
    ReExprRes r;
    r.setResVal(e);
    res.push_back(r);
    return;
  } else {
    switch(e->getKind()){
    case Expr::Reorder:{
      ReorderExpr *re = dyn_cast<ReorderExpr>(e);
      if(re -> cat == Expr::FMA_NONFMA){
	evalReorderFMANONFMA(re, res);
      }else{
	evalReorder(re, res);
      }
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

    case Expr::FOle:{
      evalFOle(dyn_cast<FOleExpr>(e), res);
      break;
    }

    case Expr::FUeq:{
      evalFUeq(dyn_cast<FUeqExpr>(e), res);
      break;
    }

    case Expr::InvalidKind:{
      assert(0 && "evaluate invalid expr");
    }

    default: {
      vector<vector<ReExprRes> > kids;
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
            if(!kids[0][i].isConflict(kids[1][j])){
	      ReExprRes r;
	      r.merge(kids[0][i], kids[1][j]);
	      tmp[0] = kids[0][i].getResVal();
	      tmp[1] = kids[1][j].getResVal();
              r.setResVal(e->rebuild(tmp));
	      res.push_back(r);
	    }
	  }
	}
      }
      return;
    }      
    }
  }
}

ReExprEvaluator::EvalState ReExprEvaluator::isAssignmentStable(const ref<Expr> &e, ref<Expr> &eps){
  bool trueRe = false, falseRe = false;
  vector<ReExprRes> res;
  evaluate(e,res);
  vector<ReExprRes>::iterator ite = res.begin();
 
  for(; ite != res.end(); ite++){
    ref<Expr> v = ite->getResVal();
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

template void ReExprEvaluator::getReorderExtreme<float>(const ReorderExpr *, vector<ReExprRes> &, vector<ReExprRes> &);
template void ReExprEvaluator::getReorderExtreme<double>(const ReorderExpr *, vector<ReExprRes> &, vector<ReExprRes> &);


