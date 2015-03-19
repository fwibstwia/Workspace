//===-- ReExprEvaluator.h ------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef KLEE_REEXPREVALUATOR_H
#define KLEE_REEXPREVALUATOR_H

#include <vector>
#include <utility>
#include <map>
#include <set>
#include <iostream>
#include <iomanip> 

#include "klee/Expr.h"
#include "klee/Reorder.h"

namespace klee{
  class ReExprEvaluator{
  public:
    typedef std::map<const Array*, std::vector<unsigned char> > bindings_ty;
    bindings_ty bindings;
    enum EvalState{
      Success = 0,
      Epsilon = 1,
      MinEqualMax = 2
    };
  private:
    void evalRead(const ReadExpr *e, std::vector<ref<Expr> > &res);
    void evalUpdate(const UpdateList &ul, unsigned index, std::vector<ref<Expr> > &res);
    void evalMultRec(const std::vector<ref<Expr> > &ops, std::vector<ref<Expr> > &kids, 
		     int i, std::vector<MultRes> &res);
    void constructMult(const ref<Expr> &src, std::vector<ref<Expr> > &ops);
    void evalReOps(const ref<Expr> &e, std::vector<std::pair<ref<Expr>, ref<Expr> > > &minVec,
		   std::vector<std::pair<ref<Expr>, ref<Expr> > > &maxVec);
    void evalReorder(const ReorderExpr *e, std::vector<ref<Expr> > &res);

    void evalFComp(const ref<Expr> &e, std::vector<ref<Expr> > &res);
    void getInitialValue(const Array &os, unsigned index, std::vector<ref<Expr> > &res); 
    ref<Expr> getArrayValue(const Array *array, unsigned index) const;
    void getResMinMax(std::vector<ref<Expr> > &res);
  private:
    ref<Expr> epsilon;
    std::map<const ReorderExpr *, std::vector<ref<Expr> > > reorderMap;
    bool isMinEqMax;
  public:
    void evaluate(const ref<Expr> &e, std::vector<ref<Expr> > &res);
    EvalState isAssignmentStable(const ref<Expr> &e, ref<Expr> &epsilon);
    ReExprEvaluator(std::vector<const Array*> &objects,
		    std::vector< std::vector<unsigned char> > &values){
      isMinEqMax = true;
      std::vector< std::vector<unsigned char> >::iterator valIt = 
        values.begin();
      for (std::vector<const Array*>::iterator it = objects.begin(),
             ie = objects.end(); it != ie; ++it) {
        const Array *os = *it;
        std::vector<unsigned char> &arr = *valIt;
        bindings.insert(std::make_pair(os, arr));
        ++valIt;
      }
    } 
  };
 
  inline void ReExprEvaluator::getInitialValue(const Array& os, 
					       unsigned index, 
					       std::vector<ref<Expr> > &res){
    ref<Expr> arrayV = getArrayValue(&os, index);
    res.push_back(arrayV);
  }

  inline ref<Expr> ReExprEvaluator::getArrayValue(const Array *array, 
                                        unsigned index) const {
    assert(array);
    bindings_ty::const_iterator it = bindings.find(array);
    unsigned offset = index * (array->range/8);
    if (it!=bindings.end() && offset<it->second.size()) {
      if(array->range == Expr::Int64){
	double v = *((double*)&it->second[offset]);
	//std::cout << array-> name << index << std::fixed << std::setprecision(17) << v << std::endl;
	return ConstantExpr::alloc(llvm::APFloat(v));
      }else if(array->range == Expr::Int32){
	float v = *((float*)&it->second[offset]);
	//std::cout << array-> name << index << std::fixed << std::setprecision(15) << v << std::endl;
	return ConstantExpr::alloc(llvm::APFloat(v));	
      }
    } else {
      return ConstantExpr::alloc(0, array->getRange());
    }
  }
  
}

#endif
















