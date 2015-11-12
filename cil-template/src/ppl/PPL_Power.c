#include <ppl.hh>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fenv.h>
#include <mpfr.h>

#include "PPL_Power.h"


#pragma STDC FENV_ACCESS ON

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
      //if(!(new_m -> power_poly).geometrically_covers(pp)){
      (new_m -> power_poly).add_disjunct(p); // omega-reduce
	//}
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

mpq_class getAbsoluteMaxVal(const FP_Interval &inv){
  mpq_class upper (abs(inv.upper()));
  mpq_class lower (abs(inv.lower()));
  if(upper > lower)
    return upper;
  return lower;
}

void setAffineFormImageReorder(PPL_Manager *manager, int vid, void *vidList, int len){
  Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();
  Pointset_Powerset<NNC_Polyhedron> update_p(manager -> dimLen, EMPTY); 
  
  while(iter != (manager -> power_poly).end()){  
    NNC_Polyhedron p = iter -> pointset();
    
    FP_Interval_Abstract_Store int_store(manager -> dimLen);
    p.refine_fp_interval_abstract_store(int_store);
    cout << "before" << p.constraints() << endl;

    const int orig_round = fegetround();

    mpq_class error = 0;
    mpq_class num_limit = numeric_limits<float>::denorm_min();
    
    Linear_Form<FP_Interval> lf;
    
    for(int i = 0; i < len; i = i + 2){
      Variable v_left = *(manager -> varIdMap[((int*)vidList)[i]]);
      Variable v_right = *(manager -> varIdMap[((int*)vidList)[i+1]]);
      FP_Interval vl_bound = int_store.get_interval(v_left);
      Linear_Form<FP_Interval> oprf(v_right);
      Linear_Form<FP_Interval> opf(vl_bound * oprf);
      error += getAbsoluteMaxVal(int_store.get_interval(v_right));
      lf = lf+ opf;
    }
    
    error = error * ((len-1) * num_limit /(1-(len-1)*num_limit));
    //convert to float
    mpfr_t q;
    mpfr_init2 (q, 24);
    mpfr_set_q(q, error.get_mpq_t(), MPFR_RNDU);
    float f_error = mpfr_get_d(q, MPFR_RNDU);
    mpfr_clear(q);
    //cout << "error are " << fixed << setprecision(50) << numeric_limits<float>::denorm_min() << endl;    
    FP_Interval error_bound_inv;
    error_bound_inv.upper() = f_error;
    error_bound_inv.lower() = -f_error;
    Linear_Form<FP_Interval> consError(error_bound_inv);
      //lf += consError;

    cout << "constraint" << lf << endl;
  
    p.affine_form_image(*(manager -> varIdMap)[vid], lf);
    cout << "after" << p.constraints() << endl;
    update_p.add_disjunct(p);
    iter ++;
  }
  manager -> power_poly = update_p;
}

Linear_Form<FP_Interval> *getLinearFormConstant(PPL_Manager *manager, float num){
  ostringstream sStream;
  sStream << fixed << setprecision(19) << num;
  string fs = sStream.str();
  const char* s = fs.c_str();
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
  //FP_Interval absError = Floating_Point_Expression<FP_Interval, IEEE754_SINGLE>::absolute_error;
  Linear_Form<FP_Interval> rlerrLeft;
  Linear_Form<FP_Interval> rlerrRight;
  left -> relative_error (ANALYZED_FP_FORMAT, rlerrLeft);
  right -> relative_error (ANALYZED_FP_FORMAT, rlerrRight);
  Linear_Form<FP_Interval> *lf = new Linear_Form<FP_Interval>(*left + *right + rlerrLeft + rlerrRight);    
  delete left;
  delete right;
  return lf;
}  

Linear_Form<FP_Interval> *getLinearFormMinus(PPL_Manager *manager, Linear_Form<FP_Interval> *left, Linear_Form<FP_Interval> *right){
  //FP_Interval absError = Floating_Point_Expression<FP_Interval, float>::absolute_error;  
  Linear_Form<FP_Interval> rlerrLeft;
  Linear_Form<FP_Interval> rlerrRight;
  left -> relative_error (ANALYZED_FP_FORMAT, rlerrLeft);
  right -> relative_error (ANALYZED_FP_FORMAT, rlerrRight);
  Linear_Form<FP_Interval> *lf = new Linear_Form<FP_Interval>(*left - *right + rlerrLeft + rlerrRight);
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
      FP_Interval_Abstract_Store int_store(manager -> dimLen);
      p.refine_fp_interval_abstract_store(int_store);
      sStream << "{" << p.constraints() << "}" << endl;
      sStream << "{" << int_store << "}" << endl;
      /*
      for(map<int Variable*>::iterator it = varIdMap.begin(); it != varIdMap.end(); it ++){
	sStream << int_store[(it -> second) -> id()] 
	
      }
      
      sStream << "}" ;*/
      iter ++;
  }
  strcpy(s, sStream.str().c_str());
  return s;
}
