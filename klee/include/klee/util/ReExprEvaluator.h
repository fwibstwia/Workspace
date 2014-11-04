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

#include "klee/Expr.h"

namespace klee{
  class Array;

  class ReExprEvaluator{
  public:
    typedef std::map<const Array*, std::vector<unsigned char> > bindings_ty;
    bindings_ty bindings;
  private:
    void evalRead(const ReadExpr *e, std::vector<ref<Expr> > &res);
    void evalReorder(const ReorderExpr *e, std::vector<ref<Expr> > &res);
    void evalFOlt(const FOltExpr *e, std::vector<ref<Expr> > &res);
    void getInitialValue(const Array& os, unsigned index, std::vector<ref<Expr> > &res); 
    void evalUpdate(const UpdateList &ul, unsigned index, std::vector<ref<Expr> > &res);
    void evalUpdate(const UpdateList &ul, std::vector<ref<Expr> > &res);
    ref<Expr> getArrayValue(const Array *array, unsigned index) const;
  private:
    ref<Expr> epsilon;
  public:
    void evaluate(const ref<Expr> &e, std::vector<ref<Expr> > &res);
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
    if (it!=bindings.end() && index<it->second.size()) {
      return ConstantExpr::alloc(it->second[index], array->getRange());
    } else {
      return ConstantExpr::alloc(0, array->getRange());
    }
  }
}

#endif
















