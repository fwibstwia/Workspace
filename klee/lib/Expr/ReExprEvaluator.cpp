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

using namespace klee;
using namespace std;

/***/

void ReExprEvaluator::evalRead(const ReadExpr *e, vector<ref<Expr> > &res){
  if((e->index)->getKind() == InvalidKind){
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
  float max, min;
  vector<float> ops;
  int len = (e->operands).size();
  Reorder<Float> ro(FE_TONEAREST);

  vector<ref<Expr> > kids;
  vector<ref<Expr> > tmp;
  for(int i = 0; i < len; i ++){
    evaluate((e->operands)[i], tmp);
    kids.push_back(tmp[0]);
    tmp.clean();
  }
  

  for(int i = 0; i < len; i ++){
    if(ConstantExpr *CE = dyn_cast<ConstantExpr>(kids[i])){
      llvm::APFloat v = CE->getAPFValue();
      ops.push_back(v.convertToFloat());
    }else{
      assert(0 && "encounter non-constant in Reorder Rebuild");
    }
  }

  switch(reCat){
  case RE_Plus:{
    max = ro.getPlusMax(ops);
    min = ro.getPlusMin(ops);
    break;
  }
  case RE_Mult:{
    max = ro.getMultMax(ops);
    min = ro.getMultMin(ops);
    break;
  }
  case RE_FMA:{
    break;
  }
  default:
    assert(0 && "unsupported reorderable expression");
  }
  APFloat apMax(max), apMin(min);
  res.push_back(ConstantExpr::Alloc(apMin));
  res.push_back(ConstantExpr::Alloc(apMax));
  return;
}

void ReExprEvaluator::evalFOlt(const FOltExpr &e, vector<ref<Expr> > &res){

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
      evalReorder(dyn_cast<ReadExpr>(e), res);
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
      for(int i = 0; i < count; i ++){
	ref<Expr> kid = e->getKid(i);
        evaluate(kid, kids[i]);
      }
      
      if(count == 1){
	for(int i = 0; i < kids[0].size(); i ++){
	  res.push_back(kids[0][i]);
	}
      } else { // for count == 2
	ref<Expr> tmp[2];
        for(int i = 0; i < kids[0].size(); i ++){	    
	  tmp[0] = kids[0][i];
	  for(int j = 0; j < kids[1].size(); j ++){
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

