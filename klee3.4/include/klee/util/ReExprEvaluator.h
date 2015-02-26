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
#include <iomanip> 

#include "klee/Expr.h"

namespace klee{
  class Array;

  class ReExprRes{
  private:
    std::set<int64_t> reorders;
    std::set<int64_t> reorderCompls;
    ref<Expr> resVal;
  public:
    ReExprRes(){}
    ReExprRes(const std::set<int64_t> &_reorders,
	      const std::set<int64_t> &_reorderCompls,
	      const ref<Expr> _resVal){
      reorders = _reorders;
      reorderCompls = _reorderCompls;
      resVal = _resVal;
    }
    
    ReExprRes(const ReExprRes &r){
      reorders = r.reorders;
      reorderCompls = r.reorderCompls;
      resVal = r.resVal;
    }
    
    bool isConflict(const ReExprRes &r){
      std::set<int64_t> v;
      std::set_intersection(reorders.begin(), reorders.end(),
			    (r.reorderCompls).begin(), (r.reorderCompls).end(),
			    std::inserter(v, v.end()));
      if(v.size() != 0)
	return true;
      return false;			    
    }

    void merge(const ReExprRes &r1, const ReExprRes &r2){
      std::set_union((r1.reorders).begin(), (r1.reorders).end(), 
		      (r2.reorders).begin(), (r2.reorders).end(), 
		     std::inserter(reorders, reorders.end()));

      std::set_union((r1.reorderCompls).begin(), (r1.reorderCompls).end(),
		     (r2.reorderCompls).begin(), (r2.reorderCompls).end(),
		     std::inserter(reorderCompls, reorderCompls.end()));
    }

    bool isFMAExpIn(){
      std::set<int64_t>::iterator it = reorders.find(0);
      return it != reorders.end();
    }

    bool isNonFMAExpIn(){
      std::set<int64_t>::iterator it = reorders.find(1);
      return it != reorders.end();
    }
    
    ref<Expr> getResVal(){
      return resVal;
    }

    void setResVal(const ref<Expr>  _resVal){
      resVal = _resVal;
    }
  }; 

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
    void evalRead(const ReadExpr *e, std::vector<ReExprRes> &res);
    void evalReorder(const ReorderExpr *e, std::vector<ReExprRes> &res);
    void evalReorderRec(const ReorderExpr *e, std::vector<ReExprRes> &res, 
			std::vector<ReExprRes> &kids, int i);
    void evalReorderFMANONFMA(const ReorderExpr *e, std::vector<ReExprRes> &res);
    void evalFMAExp(const ref<Expr> mult0, const ref<Expr> mult1, 
		    const ref<Expr> addend, std::vector<ReExprRes> &res);
    void evalNonFMAExp(const ref<Expr> mult0, const ref<Expr> mult1,
		       const ref<Expr> addend, std::vector<ReExprRes> &res);
    template <typename T>
    void getReorderExtreme(const ReorderExpr *e, 
			   std::vector<ReExprRes> &kids, 
			   std::vector<ReExprRes> &res);
    void evalFOlt(const FOltExpr *e, std::vector<ReExprRes> &res);
    void evalFOle(const FOleExpr *e, std::vector<ReExprRes> &res);
    void evalFUeq(const FUeqExpr *e, std::vector<ReExprRes> &res);
    void getInitialValue(const Array &os, unsigned index, std::vector<ReExprRes> &res); 
    void evalUpdate(const UpdateList &ul, unsigned index, std::vector<ReExprRes> &res);
    ref<Expr> getArrayValue(const Array *array, unsigned index) const;
  private:
    ref<Expr> epsilon;
    std::map<const ReorderExpr *, std::vector<ReExprRes> > reorderMap;
    bool isMinEqMax;
  public:
    void evaluate(const ref<Expr> &e, std::vector<ReExprRes> &res);
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
					       std::vector<ReExprRes> &res){
    ReExprRes re;
    ref<Expr> arrayV = getArrayValue(&os, index);
    re.setResVal(arrayV);
    res.push_back(re);
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
















