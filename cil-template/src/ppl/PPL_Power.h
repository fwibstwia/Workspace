#ifndef __PPL_POWER_H
#define __PPL_POWER_H
#include <ppl.hh>
#include <sstream>
#include "C_Expr_defs.h"


using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;

class Test_Oracle : public FP_Oracle<C_Expr,FP_Interval> {
 public:
 Test_Oracle() : int_store(0) {}

 Test_Oracle(FP_Interval_Abstract_Store init) : int_store(init) {}

  //copy constructor
  Test_Oracle(Test_Oracle & oc){
    int_store = oc.int_store;
  }

  bool get_interval(dimension_type dim, FP_Interval& result) const {
    result = int_store.get_interval(Variable(dim));
    return true;
  }

  bool get_fp_constant_value(const Floating_Point_Constant<C_Expr>& expr,
                             FP_Interval& result) const {
    result = FP_Interval((const char *)expr.value);
    return true;
  }

  bool get_integer_expr_value(const Concrete_Expression<C_Expr>& expr,
                              FP_Interval& result) const {
    if (expr.kind() == INT_CON) {
      const Integer_Constant<C_Expr>* ic_expr =
        reinterpret_cast< const Integer_Constant<C_Expr>* >(&expr);
      result = FP_Interval(ic_expr->value);
    }
    else {
      const Approximable_Reference<C_Expr>* ar_expr =
        reinterpret_cast< const Approximable_Reference<C_Expr>* >(&expr);
      result = FP_Interval(ar_expr->value);
    }

    return true;
  }

  bool get_associated_dimensions(
       const Approximable_Reference<C_Expr>& expr,
       std::set<dimension_type>& result) const {
    result = expr.dimensions;
    return true;
  }

  FP_Interval_Abstract_Store int_store;
};

//TODO: Merge PowerSet

class PPL_Manager {
 public:
 PPL_Manager(int *varIdArray, int len): dimLen(len), oracle(FP_Interval_Abstract_Store(len)), power_poly(len, EMPTY) {

    for(int i = 0; i < dimLen; i ++){
      varIdMap[varIdArray[i]] = new Variable(i);
    }
    FP_Octagonal_Shape p(len, UNIVERSE);
    //add constraints
    p.add_constraint(*varIdMap[varIdArray[0]] >= 0);
    p.add_constraint(*varIdMap[varIdArray[0]] <= 2);
    p.add_constraint(*varIdMap[varIdArray[1]] >= 0);
    p.add_constraint(*varIdMap[varIdArray[1]] <= 2);
    p.add_constraint(*varIdMap[varIdArray[2]] >= 0);
    p.add_constraint(*varIdMap[varIdArray[2]] <= 2);
    
    power_poly.add_disjunct(p);
    p.refine_fp_interval_abstract_store(oracle.int_store);
  }

 PPL_Manager(PPL_Manager &manager): dimLen(manager.dimLen), oracle(manager.oracle), power_poly(manager.power_poly), varIdMap(manager.varIdMap){    
  }

  map<int, Variable*> varIdMap; //map vid to PPL Variable Type
  Test_Oracle oracle;
  FP_Linear_Form_Abstract_Store lf_abstract_store;
  Pointset_Powerset<FP_Octagonal_Shape> power_poly;
  int dimLen;
};

Concrete_Expression_Type FP_Type =
  Concrete_Expression_Type::floating_point(ANALYZED_FP_FORMAT);



#ifdef __cplusplus
extern "C" {
#endif
  PPL_Manager *init(void *varIdArray, int dimLen){
    return new PPL_Manager((int*)varIdArray, dimLen);
  }
  
  PPL_Manager *copy(PPL_Manager *manager){
    return new PPL_Manager(*manager);
  }// copy PPL_Manager;
  
  bool merge(PPL_Manager *old_m, PPL_Manager *new_m); // merge PPL_Manager return whether the state has changed
  void setAffineFormImage(PPL_Manager *manager, int vid, Linear_Form<FP_Interval> *lf);
  void setAffineFormImageReorder(PPL_Manager *manager, int vid, void *vidList, int len);
  Linear_Form<FP_Interval> *getLinearFormConstant(PPL_Manager *manager, float num);
  Linear_Form<FP_Interval> *getLinearFormVariable(PPL_Manager *manager, int vid);
  Linear_Form<FP_Interval> *getLinearFormPlus(PPL_Manager *manager, Linear_Form<FP_Interval> *left, Linear_Form<FP_Interval> *right);
  Linear_Form<FP_Interval> *getLinearFormMinus(PPL_Manager *manager, Linear_Form<FP_Interval> *left, Linear_Form<FP_Interval> *right);
  void addConstraint(PPL_Manager *manager, Linear_Form<FP_Interval> *left, Linear_Form<FP_Interval> *right);
  char *getConstraintPretty(PPL_Manager *manager);
#ifdef __cplusplus
}
#endif

#endif
