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

void setAffineFormImage(PPL_Manager *manager, int vid, DP_Form *dp_f){
  Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();
  Pointset_Powerset<NNC_Polyhedron> update_p(manager -> dimLen, EMPTY); 
  
  while(iter != (manager -> power_poly).end()){
    NNC_Polyhedron p = iter -> pointset();
    FP_Interval_Abstract_Store inv_store(manager -> dimLen);
    p.refine_fp_interval_abstract_store(inv_store);

    Linear_Form<FP_Interval> lf_lower;
    Linear_Form<FP_Interval> lf_upper;

    dp_f -> convertToLC(manager, inv_store, lf_lower, lf_upper, vid);
    Linear_Form<FP_Interval> vf(*(manager->varIdMap)[vid]);
    p.unconstrain(*(manager->varIdMap)[vid]);
    p.refine_with_linear_form_inequality(lf_lower, vf);
    p.refine_with_linear_form_inequality(vf, lf_upper);
    update_p.add_disjunct(p);
    iter ++;
  }
  manager -> power_poly = update_p;
  delete dp_f;
}


DP_Form *getDPFormConstant(PPL_Manager *manager, float num){
  return new DP_Form(num);
}

DP_Form *getDPFormVariable(PPL_Manager *manager, int vid){
  return new DP_Form(vid);
}

DP_Form *getDPFormPlus(PPL_Manager *manager, DP_Form *left, DP_Form *right){
  DP_Form *vlf = new DP_Form(*left + *right);  
  delete left;
  delete right;
  return vlf;
}

DP_Form *getDPFormMinus(PPL_Manager *manager, DP_Form *left, DP_Form *right){  
  DP_Form *vlf = new DP_Form(*left - *right);  
  delete left;
  delete right;
  return vlf;
}

DP_Form *getDPFormTimes(PPL_Manager *manager, DP_Form *left, DP_Form *right){//parenthesis free
  pair<vector<float>, vector<int> > p = left -> getLk();				     
  DP_Form *vlf = new DP_Form(p * (*right));  
  delete left;
  delete right;
  return vlf;
}

/* add constraint: A <= 0 */
void addConstraint(PPL_Manager *manager, DP_Form *left, DP_Form *right){
    Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();    
    Pointset_Powerset<NNC_Polyhedron> update_p(manager -> dimLen, EMPTY);

    int vid = ((left->getLk()).second)[0];
    float c = ((right->getLk()).first)[0];
    FP_Interval inv_c;
    inv_c.lower() = c;
    inv_c.upper() = c;
    Variable v = *(manager -> varIdMap[vid]);
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

char *getConstraintPretty(PPL_Manager *manager){
  ostringstream sStream;
  Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();
  static char *s = new char[10000];
  while(iter != (manager -> power_poly).end()){
      NNC_Polyhedron p = iter -> pointset();
      FP_Interval_Abstract_Store inv_store(manager -> dimLen);
      p.refine_fp_interval_abstract_store(inv_store);
      sStream << "{" << p.constraints() << "}" << endl;
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
