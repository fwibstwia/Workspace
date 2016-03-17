#include <ppl.hh>
#include <sstream>
#include <iostream>

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
      //if(!(new_m -> power_poly).geometrically_covers(pp)){
      (new_m -> power_poly).add_disjunct(p); // omega-reduce
	//}
      iter ++;
    }
    delete old_m;
    return false;
  }
}
/*
  D = A*B + C
 */
void setAffineFormImage(PPL_Manager *manager, int vid, Polynomial *poly){
  Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();
  Pointset_Powerset<NNC_Polyhedron> update_p(manager -> dimLen, EMPTY); 
  
  while(iter != (manager -> power_poly).end()){
    NNC_Polyhedron p = iter -> pointset();
    FP_Interval_Abstract_Store inv_store(manager -> dimLen);
    p.refine_fp_interval_abstract_store(inv_store);

    NNC_Polyhedron poly_p = poly -> polyhedronApprox(inv_store,  (*(manager->varIdMap)[vid]).id());
    p.unconstrain(*(manager->varIdMap)[vid]);
    p.intersection_assign (poly_p);
    /*
    Linear_Form<FP_Interval> lf_lower;
    Linear_Form<FP_Interval> lf_upper;

    dp_f -> convertToLC(manager, inv_store, lf_lower, lf_upper, vid);
    
    Linear_Form<FP_Interval> vf(*(manager->varIdMap)[vid]);
    
    p.unconstrain(*(manager->varIdMap)[vid]);
    
    p.refine_with_linear_form_inequality(lf_lower, vf);
    
    p.refine_with_linear_form_inequality(vf, lf_upper);*/
    
    update_p.add_disjunct(p);
    iter ++;
  }
  manager -> power_poly = update_p;
  delete poly;
}

/*return true, we have found fake counter example; return false, continue the refine process*/
bool refineBadState(PPL_Manager *manager, int vid, Polynomial *poly){
  Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();
  Pointset_Powerset<NNC_Polyhedron> update_p(manager -> dimLen, EMPTY); 

  /*TODO:currently we assume only one branch, thus the powerset contains just one polyhedron */
  while(iter != (manager -> power_poly).end()){
    NNC_Polyhedron p = iter -> pointset();
    FP_Interval_Abstract_Store inv_store(manager -> dimLen);
    p.refine_fp_interval_abstract_store(inv_store);
    NNC_Polyhedron poly_p(manager->dimLen);
    int count = 0;
    while(count < 10){
      poly_p = poly -> polyhedronApprox(inv_store,  (*(manager->varIdMap)[vid]).id());

      cout << "before:" <<  inv_store << endl;
      p.intersection_assign(poly_p);
      FP_Interval_Abstract_Store inv_store_test(manager -> dimLen);
      p.refine_fp_interval_abstract_store(inv_store_test);
      cout << "intersec: " << inv_store_test << endl;
      
      if(p.is_empty()){
	cout << "success SSSSSSSSSSSSSSSSSSSSS" << endl;
	return true;
      }
      p.refine_fp_interval_abstract_store(inv_store);
      cout << "after:" <<  inv_store << endl;
      count ++;
    }

    //p.unconstrain(*(manager->varIdMap)[vid]);    
    update_p.add_disjunct(p);
    iter ++;
  }
  
  manager -> power_poly = update_p;
  cout << "new:" << getConstraintPretty(manager) << endl;
  delete poly;
  return false;
}


Polynomial *getPolynomialConstant(PPL_Manager *manager, float num){
  return new Polynomial(num, manager -> dimLen);
}

Polynomial *getPolynomialVariable(PPL_Manager *manager, int vid){
  
  Polynomial *p = new Polynomial((int)(*(manager->varIdMap)[vid]).id(), manager -> dimLen);
  return p;
}

Polynomial *getPolynomialPlus(PPL_Manager *manager, Polynomial *left, Polynomial *right){
  Polynomial *p = new Polynomial(*left + *right);  
  delete left;
  delete right;
  return p;
}

Polynomial *getPolynomialUnaryMinus(PPL_Manager *manager, Polynomial *l){
  Polynomial *minusOne = getPolynomialConstant(manager, -1);
  Polynomial *r = getPolynomialTimes(manager, minusOne, l);
  return r;
}

Polynomial *getPolynomialMinus(PPL_Manager *manager, Polynomial *left, Polynomial *right){  
  Polynomial *p = new Polynomial(*left - *right);  
  delete left;
  delete right;
  return p;
}

Polynomial *getPolynomialTimes(PPL_Manager *manager, Polynomial *left, Polynomial *right){//parenthesis free
				     
  Polynomial *p = new Polynomial((left->monomial_list)[0] * (*right));
  delete left;
  delete right;
  return p;
}

/* add constraint: A <= 0 */
//TODO : need to use linear constraint, instead of linear form constraint
void addConstraint(PPL_Manager *manager, Polynomial *left, Polynomial *right){
    Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();    
    Pointset_Powerset<NNC_Polyhedron> update_p(manager -> dimLen, EMPTY);
    int dim = 0;
    float c = 0.0;
    
    if(((left->monomial_list[0]).coefficients).size() > 0){
      c = (left -> monomial_list[0]).coefficients[0].get_d();
      for(int j = 0; j < right -> dimLen; j ++){
	if((dim = right -> monomial_list[0].m_degree[j]) != 0){
	  break;
	}
      }
    } else {
      c = (right -> monomial_list[0]).coefficients[0].get_d();
      for(int j = 0; j < left -> dimLen; j ++){
	if((dim = left -> monomial_list[0].m_degree[j]) != 0){
	  break;
	}
      }      
    }
      
    FP_Interval inv_c;
    inv_c.lower() = c;
    inv_c.upper() = c;
    Variable v(dim);
    
    Linear_Form<FP_Interval> lf_v(v), lf_c(inv_c);
    
    while(iter != (manager -> power_poly).end()){
      NNC_Polyhedron p = iter -> pointset();      
      p.refine_with_linear_form_inequality(lf_v, lf_c);
      update_p.add_disjunct(p);
      iter ++;
    }
    
    manager -> power_poly = update_p;
    delete left;
    delete right;
}

//TODO: see above addConstraint
void evalEqualConstraint(PPL_Manager *manager, Polynomial *left, Polynomial *right){
    Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();    
    Pointset_Powerset<NNC_Polyhedron> update_p(manager -> dimLen, EMPTY);
    int dim = 0;
    float c = 0.0;
    
    if(((left->monomial_list[0]).coefficients).size() > 0){
      c = (left -> monomial_list[0]).coefficients[0].get_d();
      for(int j = 0; j < right -> dimLen; j ++){
	if((dim = right -> monomial_list[0].m_degree[j]) != 0){
	  break;
	}
      }
    } else {
      c = (right -> monomial_list[0]).coefficients[0].get_d();
      for(int j = 0; j < left -> dimLen; j ++){
	if((dim = left -> monomial_list[0].m_degree[j]) != 0){
	  break;
	}
      }      
    }
      
    FP_Interval inv_c;
    inv_c.lower() = c;
    inv_c.upper() = c;
    cout << "dim: " << dim << endl;
    Variable v(10);
    
    Linear_Form<FP_Interval> lf_v(v), lf_c(inv_c);
    cout << "before:" << getConstraintPretty(manager) << endl;
    while(iter != (manager -> power_poly).end()){
      NNC_Polyhedron p = iter -> pointset();
      //refine with linear equality lf_v == lf_c
      //p.refine_with_linear_form_inequality(lf_v, lf_c);
      //p.refine_with_linear_form_inequality(lf_c, lf_v);
      //HACK: Fix K == 0
      p.add_constraint(v == 0);
      update_p.add_disjunct(p);
      iter ++;
    }
    
    manager -> power_poly = update_p;
    cout << "after:" << getConstraintPretty(manager) << endl;
    delete left;
    delete right;
}


char *getConstraintPretty(PPL_Manager *manager){
  ostringstream sStream;
  Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();
  static char *s = new char[10000];
  while(iter != (manager -> power_poly).end()){
      NNC_Polyhedron p = iter -> pointset();
      FP_Interval_Abstract_Store inv_store(manager -> dimLen);
      p.refine_fp_interval_abstract_store(inv_store);
      //sStream << "{" << p.constraints() << "}" << endl;
      sStream << "{" << inv_store << "}" << endl;
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
