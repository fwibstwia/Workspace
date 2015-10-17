#include <ppl.hh>
#include <sstream>
#include "PPL_Power.h"

using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;

PPL_Manager *init(int *varIdArray, int dimLen){
  
}
/* Check new powerset is the same as old powerset, return true
   else merge the two powerset, delete old_m, return false
 */
bool merge(PPL_Manager *new_m, PPL_Manager *old_m){
  if((new_m ->power_poly).geometrically_equals(old_m -> power_poly)){
    delete old_m;
    return true;
  }else{
    Pointset_Powerset<NNC_Polyhedron>::const_iterator iter = (old_m -> power_poly).begin();
    while(iter != (old_m -> power_poly).end()){
      Pointset_Powerset<NNC_Polyhedron> pp(*iter);
      if(!(new_m -> power_poly).geometrically_covers(pp)){
	(new_m -> power_poly).add_disjunct(*iter);
      }
      iter ++;
    }
    delete old_m;
    return false;
  }
}

void setAffineFormImage(PPL_Manager *manager, int vid, Linear_Form<FP_Interval> *lf){
  Pointset_Powerset<NNC_Polyhedron>::iterator iter = (manager -> power_poly).begin();
  while(iter != (old_m -> power_poly).end()){
    *iter.affine_form_image(*varIdMap[vid], *lf);
    iter ++;
  }
  delete lf;
}

Linear_Form<FP_Interval> *getLinearFormPlus(PPL_Manager *manager, Linear_Form<FP_Interval> *left, Linear_Form<FP_Interval> *right){
  delete left;
  delete right;
}  

Linear_Form<FP_Interval> *getLinearFormMinus(PPL_Manager *manager, Linear_Form<FP_Interval> *left, Linear_Form<FP_Interval> *right){
  delete left;
  delete right;
}
/* add constraint: left <= right */
void addConstraint(PPL_Manager *manager, Linear_Form<FP_Interval> *left, Linear_Form<FP_Interval *right){
}
