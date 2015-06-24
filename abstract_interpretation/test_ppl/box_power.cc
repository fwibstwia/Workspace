#include <ppl.hh>
#include <iostream>
#include "C_Expr_defs.hh"

using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;

class Test_Oracle : public FP_Oracle<C_Expr,FP_Interval> {
public:
  Test_Oracle() : int_store(0) {}

  Test_Oracle(FP_Interval_Abstract_Store init) : int_store(init) {}

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

/*
A \in [0,1]
   B \in [0,4]
   A + B <= 2
if(z > 0){
  A = A + 5;
}else{
  A = A - 1;
}
if(A + B >= 3 && A + B <= 4){

}*/

// Generate FP_Linear_abstract_Store
// Generate FP_Linear_Form
// Implement FP_Oracle
// Generate Box_Powerset

Concrete_Expression_Type FP_Type =
  Concrete_Expression_Type::floating_point(ANALYZED_FP_FORMAT);

void test_power_interval(){

  Variable A(0);
  Variable B(1);
  Constraint_System cs;
  cs.insert(A >= 0);
  cs.insert(A <= 1);
  cs.insert(B >= 0);
  cs.insert(B <= 4);
  cs.insert(A + B <= 2);

  TBox b(2);
  b.refine_with_constraints(cs);

  Pointset_Powerset<TBox> ps_b(2, EMPTY);
  TBox b1(b);
  FP_Interval free_term(static_cast<FP_Interval::boundary_type>(5));
  free_term.join_assign(5);
  FP_Linear_Form l(A);
  l += free_term;
  b1.affine_form_image(A, l);
  ps_b.add_disjunct(b1);
  TBox b2(b);
  ps_b.add_disjunct(b2);

  ps_b.refine_with_constraint(A + B >= 3);
  ps_b.refine_with_constraint(A + B <= 4);

  Pointset_Powerset<TBox>::const_iterator i0 = ps_b.begin();
  TBox bi1 = i0->pointset();
  cout << "*** dist_box_1.constraints ***" << bi1.constraints() << endl;
  i0 ++;
  TBox bi2 = i0->pointset();
  cout << "*** dist_box_2.constraints ***" << bi2.constraints() << endl;

  bool ok = ps_b.is_empty();
  cout << "*** is_empty *** " << ok << endl;
}

void test_power_oct(){
  Variable A(0);
  Variable B(1);
  Constraint_System cs;
  cs.insert(A >= 0);
  cs.insert(A <= 1);
  cs.insert(B >= 0);
  cs.insert(B <= 4);
  cs.insert(A + B <= 2);

  TOctagonal_Shape oc(2);
  oc.add_constraints(cs);

  Pointset_Powerset<TOctagonal_Shape> ps_o(2, EMPTY);
  TOctagonal_Shape oc1(oc);
  FP_Interval free_term1(static_cast<FP_Interval::boundary_type>(5));
  free_term1.join_assign(5);
  FP_Linear_Form l1(A);
  l1 += free_term1;
  oc1.affine_form_image(A, l1);
  ps_o.add_disjunct(oc1);
  TOctagonal_Shape oc2(oc);
  ps_o.add_disjunct(oc2);

  ps_o.add_constraint(A + B >= 3);
  ps_o.add_constraint(A + B <= 4);


  Pointset_Powerset<TOctagonal_Shape>::const_iterator i = ps_o.begin();
  TOctagonal_Shape osi1 = i->pointset();
  cout << "*** dist_oct1.constraints ***" << osi1.constraints() << endl;
  i ++;
  TOctagonal_Shape osi2 = i->pointset();
  cout << "*** dist_oct2.constraints ***" << osi2.constraints() << endl;

  bool ok = ps_o.is_empty();
  cout << "*** is_empty *** " << ok << endl;
}

void test_fp_power_oct(){
  Variable A(0);
  Variable B(1);
  //Initial input constraint
  Constraint_System cs;
  cs.insert(A >= 0);
  cs.insert(A <= 1);
  cs.insert(B >= 0);
  cs.insert(B <= 4);

  TOctagonal_Shape oc(2);
  oc.add_constraints(cs);

  //oracle store
  FP_Interval tmp;
  Test_Oracle oracle(FP_Interval_Abstract_Store(2));
  tmp.lower() =  FP_Interval::boundary_type(0);
  tmp.upper() =  FP_Interval::boundary_type(1);
  oracle.int_store.set_interval(A, tmp);
  Approximable_Reference<C_Expr> var0(FP_Type, tmp, 0);

  tmp.lower() =  FP_Interval::boundary_type(0);
  tmp.upper() =  FP_Interval::boundary_type(4);
  oracle.int_store.set_interval(B, tmp);
  Approximable_Reference<C_Expr> var1(FP_Type,
                      tmp, 1);

  //linear form constaint A + B <= 2
  Binary_Operator<C_Expr> sum(FP_Type, Binary_Operator<C_Expr>::ADD,
                              &var0, &var1);
  FP_Linear_Form result;
  linearize(sum, oracle, FP_Linear_Form_Abstract_Store(), result);
  tmp.lower() = FP_Interval::boundary_type(2);
  tmp.upper() = FP_Interval::boundary_type(2);
  FP_Linear_Form lc(tmp);
  oc.refine_with_linear_form_inequality(result, lc);

  //generate two fp octagons
  Pointset_Powerset<TOctagonal_Shape> ps_o(2, EMPTY);
  TOctagonal_Shape oc1(oc);
  TOctagonal_Shape oc2(oc);

  //oc1 A = A + 5;
  Floating_Point_Constant<C_Expr> fp_c("5", 2);
  Binary_Operator<C_Expr> sum1(FP_Type, Binary_Operator<C_Expr>::ADD,
                              &var0, &fp_c);
  FP_Linear_Form result1;
  linearize(sum1, oracle, FP_Linear_Form_Abstract_Store(), result1);
  oc1.affine_form_image(A, result1);

  //add disjuncts
  ps_o.add_disjunct(oc1);
  ps_o.add_disjunct(oc2);
  TOctagonal_Shape oc3(oc1);
  oc3.upper_bound_assign(oc2);

  //check constraints: A + B >= 3; A + B <= 4
  tmp.lower() = FP_Interval::boundary_type(3);
  tmp.upper() = FP_Interval::boundary_type(3);
  FP_Linear_Form lc1(tmp);
  oc1.refine_with_linear_form_inequality(lc1, result);
  oc2.refine_with_linear_form_inequality(lc1, result);
  oc3.refine_with_linear_form_inequality(lc1, result);

  tmp.lower() = FP_Interval::boundary_type(4);
  tmp.upper() = FP_Interval::boundary_type(4);
  FP_Linear_Form lc2(tmp);
  oc1.refine_with_linear_form_inequality(result, lc2);
  oc2.refine_with_linear_form_inequality(result, lc2);
  oc3.refine_with_linear_form_inequality(lc1, result);

  bool ok;
  ok = oc1.is_empty();
  cout << "*** oc1 is_empty *** " << ok << endl;
  ok = oc2.is_empty();
  cout << "*** oc2 is_empty *** " << ok << endl;
  ok = oc3.is_empty();
  cout << "*** oc1 join oc2 is_empty *** " << ok << endl;
  /*
  Pointset_Powerset<TOctagonal_Shape>::const_iterator i;

  for(i = ps_o.begin(); i != ps_o.end(); i ++){
       TOctagonal_Shape osi = i->pointset();
       tmp.lower() = FP_Interval::boundary_type(3);
       tmp.upper() = FP_Interval::boundary_type(3);
       FP_Linear_Form lc1(tmp);
       osi.refine_with_linear_form_inequality(lc1, result);


       tmp.lower() = FP_Interval::boundary_type(4);
       tmp.upper() = FP_Interval::boundary_type(4);
       FP_Linear_Form lc2(tmp);
       osi.refine_with_linear_form_inequality(result, lc2);

       cout << "*** dist_oct1.constraints ***" << osi.constraints() << endl;
       bool ok = osi.is_empty();
       cout << "*** is_empty *** " << ok << endl;
       }*/

}

int main(){
  test_fp_power_oct();
  return 0;
}
