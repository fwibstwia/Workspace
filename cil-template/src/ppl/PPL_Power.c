#include <ppl.hh>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "PPL_Power.h"

using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;


/* Check new powerset is the same as old powerset, return true
   else merge the two powerset, delete old_m, return false
 */
bool merge(PPL_Manager *old_m, PPL_Manager *new_m){
  if((new_m ->power_poly).geometrically_equals(old_m -> power_poly)){
    delete old_m;  /*just for test*/
    return true;
  }else{
    Pointset_Powerset<NNC_Polyhedron>::const_iterator iter = (old_m -> power_poly).begin();
    while(iter != (old_m -> power_poly).end()){
      NNC_Polyhedron p = iter -> pointset();
      Pointset_Powerset<NNC_Polyhedron> pp(p);
      if(!(new_m -> power_poly).geometrically_covers(pp)){
	(new_m -> power_poly).add_disjunct(p);
      }
      iter ++;
    }
    delete old_m;
    return false;
  }
}

void setAffineFormImage(PPL_Manager *manager, int vid, Linear_Form<FP_Interval> *lf){
  Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();
  Pointset_Powerset<NNC_Polyhedron> update_p(manager -> dimLen, EMPTY); 
  
  while(iter != (manager -> power_poly).end()){
    NNC_Polyhedron p = iter -> pointset();
    p.affine_form_image(*(manager -> varIdMap)[vid], *lf);
    update_p.add_disjunct(p);
    iter ++;
  }
  manager -> power_poly = update_p;
  delete lf;
}


Linear_Form<FP_Interval> *getLinearFormConstant(PPL_Manager *manager, float num){
  ostringstream sStream;
  sStream << fixed << setprecision(19) << num;
  string fs = sStream.str();
  const char* s = fs.c_str();
  cout << fs << endl;
  Floating_Point_Constant<C_Expr> con(s, strlen(s));

  Linear_Form<FP_Interval> clf;
  linearize(con, manager -> oracle, manager->lf_abstract_store, clf);
  return new Linear_Form<FP_Interval>(clf);
}

Linear_Form<FP_Interval> *getLinearFormVariable(PPL_Manager *manager, int vid){
  return new Linear_Form<FP_Interval>(*(manager -> varIdMap[vid]));
}

Linear_Form<FP_Interval> *getLinearFormPlus(PPL_Manager *manager, Linear_Form<FP_Interval> *left, Linear_Form<FP_Interval> *right){
  //don't understand why need the following method
  /*  Approximable_Reference<C_Expr> px(FP_Type, Int_Interval(mpz_class(0)),
                                      X.id());
    Approximable_Reference<C_Expr> ps(FP_Type, Int_Interval(mpz_class(0)),
                                      S.id());
    Binary_Operator<C_Expr> x_dif_s(FP_Type, Binary_Operator<C_Expr>::SUB,
                                    &px, &ps);
    lin_success = linearize(x_dif_s, oracle, lf_abstract_store, lr);
    oracle.int_store.affine_form_image(R, FP_Linear_Form(X - S));
    if (lin_success) {
      ph.affine_form_image(R, lr);
      }*/
  
  Linear_Form<FP_Interval> *lf = new Linear_Form<FP_Interval>(*left + *right);
    
  delete left;
  delete right;
  return lf;
}  

Linear_Form<FP_Interval> *getLinearFormMinus(PPL_Manager *manager, Linear_Form<FP_Interval> *left, Linear_Form<FP_Interval> *right){

  Linear_Form<FP_Interval> *lf = new Linear_Form<FP_Interval>(*left + *right);
  delete left;
  delete right;
  return lf;
}
/* add constraint: left <= right */
void addConstraint(PPL_Manager *manager, Linear_Form<FP_Interval> *left, Linear_Form<FP_Interval> *right){
    Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();
    
    Pointset_Powerset<NNC_Polyhedron> update_p(manager -> dimLen, EMPTY); 
  
    while(iter != (manager -> power_poly).end()){
      NNC_Polyhedron p = iter -> pointset();
      p.refine_with_linear_form_inequality(*left, *right);
      p.refine_fp_interval_abstract_store((manager -> oracle).int_store); /* need to merge the abstract store */
      update_p.add_disjunct(p);
      iter ++;
    }
    manager -> power_poly = update_p;
    delete left;
    delete right;
}

char *getConstraintPretty(PPL_Manager *manager){
  ostringstream sStream;
  Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();
  static char *s = new char[10000];
  while(iter != (manager -> power_poly).end()){
      NNC_Polyhedron p = iter -> pointset();
      sStream << "{" << p.constraints() << "}" ;
      iter ++;
  }
  strcpy(s, sStream.str().c_str());
  return s;
}
