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
#include <map>
#include <set>
#include <iostream>

#include "klee/Expr.h"

namespace klee{
  class Array;

  class ReExprRes{
  private:
    std::set<ref<Expr> > contains;
    std::set<ref<Expr> > complements;
  public:
    ReExprRes(const std::set<ref<Expr> > &_contains, 
	      const std::set<ref<Expr> > &_complements){
      contains = _contains;
      complements = _complements;
    } 
    
    ReExprRes(const ReExprRes &r){
      contains = r.contains;
      complements = r.complements;
    }
    
    bool isConflict(const ReExprRes &r){
      
    }

    void merge(const ReExprRes &){

    }
  }; 

  class ReExprEvaluator{
  public:
    typedef std::map<const Array*, std::vector<unsigned char> > bindings_ty;
    bindings_ty bindings;
  private:
    void evalRead(const ReadExpr *e, std::vector<ReExprRes> &res);
    void evalReorder(const ReorderExpr *e, std::vector<ReExprRes> &res);
    void evalFOlt(const FOltExpr *e, std::vector<ReExprRes> &res);
    void getInitialValue(const Array& os, unsigned index, std::vector<ReExprRes> &res); 
    void evalUpdate(const UpdateList &ul, unsigned index, std::vector<ReExprRes> &res);
    ref<Expr> getArrayValue(const Array *array, unsigned index) const;
  private:
    ref<Expr> epsilon;
  public:
    void evaluate(const ref<Expr> &e, std::vector<ref<Expr> > &res);
    bool isAssignmentStable(const ref<Expr> &e, ref<Expr> &epsilon);
    ReExprEvaluator(std::vector<const Array*> &objects,
		    std::vector< std::vector<unsigned char> > &values){
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
    res.push_back(getArrayValue(&os, index));
    return;
  }

  inline ref<Expr> ReExprEvaluator::getArrayValue(const Array *array, 
                                        unsigned index) const {
    assert(array);
    bindings_ty::const_iterator it = bindings.find(array);
    unsigned offset = index * (array->range/8);
    if (it!=bindings.end() && offset<it->second.size()) {
      //just for float
      float v = *((float*)&it->second[offset]);
      return ConstantExpr::alloc(llvm::APFloat(v));
    } else {
      return ConstantExpr::alloc(0, array->getRange());
    }
  }
}

#endif
















