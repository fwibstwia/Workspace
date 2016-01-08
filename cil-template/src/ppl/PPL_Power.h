#ifndef __PPL_POWER_H
#define __PPL_POWER_H

#include <sstream>

#include "PPL_Manager.h"
#include "DP_Form.h"


using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;

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


  void setAffineFormImage(PPL_Manager *manager, int vid, DP_Form *dp_f);
  DP_Form *getDPFormConstant(PPL_Manager *manager, float num);
  DP_Form *getDPFormVariable(PPL_Manager *manager, int vid);
  DP_Form *getDPFormPlus(PPL_Manager *manager, DP_Form *left, DP_Form *right);
  DP_Form *getDPFormMinus(PPL_Manager *manager, DP_Form *left, DP_Form *right);
  DP_Form *getDPFormTimes(PPL_Manager *manager, DP_Form *left, DP_Form *right);
  void addConstraint(PPL_Manager *manager, DP_Form *left, DP_Form *right);
  char *getConstraintPretty(PPL_Manager *manager);
#ifdef __cplusplus
}
#endif

#endif
