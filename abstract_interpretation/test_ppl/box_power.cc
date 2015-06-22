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

int main(){
  //-----------------Floating-Point arithmetics ---------//  
  Variable A(0);
  Variable B(1);
  TOctagonal_Shape oc3(2);
  oc3.add_constraint(A + B <= 5);

  Test_Oracle oracle(FP_Interval_Abstract_Store(2));
  FP_Interval tmp(static_cast<FP_Interval::boundary_type>(0));
  tmp.join_assign(1);

  Approximable_Reference<C_Expr> var0(FP_Type, 
                     FP_Interval(static_cast<FP_Interval::boundary_type>(0)), 0);
  oracle.int_store.set_interval(A, tmp);

  tmp.lower() =  FP_Interval::boundary_type(0);
  tmp.upper() =  FP_Interval::boundary_type(4);
  Approximable_Reference<C_Expr> var1(FP_Type, 
                  FP_Interval(static_cast<FP_Interval::boundary_type>(0)), 1);
  oracle.int_store.set_interval(B, tmp);
  

  Binary_Operator<C_Expr> sum(FP_Type, Binary_Operator<C_Expr>::ADD,
                              &var0, &var1);
  FP_Linear_Form result;
  linearize(sum, oracle, FP_Linear_Form_Abstract_Store(), result);

  oc3.affine_form_image(A, result); 
  //Floating_Point_Constant<C_Expr> num("2", 2);
  //FP_Linear_Form c = FP_Linear_Form(num);
  //FP_Interval c(2);
  //FP_Linear_Form lc(c);
 
  //oc3.refine_with_linear_form_inequality(result, lc);

  cout << "*** fp_arith.constraints ***" << oc3.constraints() << endl;
  //-----------------Floating-point Power domain -------//
  











  //----------------------------------------------------
  /*
  Variable x(0);
  Variable y(1);

  FP_Interval_Abstract_Store i_store(2);
  FP_Linear_Form_Abstract_Store l_store;
  
  FP_Interval tmp(static_cast<FP_Interval::boundary_type>(0));
  tmp.join_assign(static_cast<FP_Interval::boundary_type>(1));
  i_store.set_interval(x, tmp);

  tmp.lower() = 0;
  tmp.upper() = 4;
  i_store.set_interval(y, tmp);
 
  cout << "*** i_store.constraints ***" << i_store.constraints() << endl;

  Binary_Operator<C_Expr> div(FP_Type, Binary_Operator<C_Expr>::DIV, &num, &den);


  //Concrete_Expression<C_Expr> ce;

  
  

  

  /*x
  Constraint_System cs;
  cs.insert(x >= 0);
  cs.insert(x <= 3);
  cs.insert(y >= 0);
  cs.insert(y <= 4);
  //cs.insert(x + y <= 3);
  // --- Powerset Box Constraints
  TBox box(2);
  box.propagate_constraints(cs);
  Linear_Expression le = x + y;
  
  box.affine_image(x, le);
  //box.refine_with_constraint(x == x + y);

  cout << "*** box.constraints ***" << box.constraints() << endl;

  Pointset_Powerset<TBox> ps_b(2, EMPTY);

  TBox box1(box);
  Linear_Expression le = x + y;
  Linear_Expression lx = x;
  lx = le;

  box1.refine_with_constraint(lx <= 2);
  ps_b.add_disjunct(box1);
  
  TBox box2(box);
  box2.add_constraint(x <= 1);
  ps_b.add_disjunct(box2);

  ps_b.refine_with_constraint (x <= 0);
  //ps_b.refine_with_constraint(x + y >= 3);
  //ps_b.refine_with_constraint(x + y <= 4);


  bool ok = ps_b.is_empty();

  Pointset_Powerset<TBox>::const_iterator c_i = ps_b.begin();
  TBox b_phi = c_i->pointset();
  cout << "*** box.constraints ***" << b_phi.constraints() << endl;
  c_i ++;
  b_phi = c_i->pointset();
  cout << "*** box.constraints ***" << b_phi.constraints() << endl;

  // --- Powerset Octogon Constraints
  TOctagonal_Shape oct(2);
  oct.add_constraints(cs);
 
  oct.add_constraint(x == x + 1);

  Pointset_Powerset<TOctagonal_Shape> ps_o(2, EMPTY);

  ok = oct.is_empty();

  cout << "*** oct.propagate_constraints(cs) ***" << oct.constraints() << endl;
  */

  return 0;
}

