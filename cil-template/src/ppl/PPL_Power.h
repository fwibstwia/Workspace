#ifndef __PPL_POWER_H
#define __PPL_POWER_H

#include <sstream>

#include "PPL_Manager.h"
#include "Polynomial.h"


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


  void setAffineFormImage(PPL_Manager *manager, int vid, Polynomial *dp_f);
  Polynomial *getPolynomialConstant(PPL_Manager *manager, float num);
  Polynomial *getPolynomialVariable(PPL_Manager *manager, int vid);
  Polynomial *getPolynomialPlus(PPL_Manager *manager, Polynomial *left, Polynomial *right);
  Polynomial *getPolynomialUnaryMinus(PPL_Manager *manager, Polynomial *left);
  Polynomial *getPolynomialMinus(PPL_Manager *manager, Polynomial *left, Polynomial *right);
  Polynomial *getPolynomialTimes(PPL_Manager *manager, Polynomial *left, Polynomial *right);
  void addConstraint(PPL_Manager *manager, Polynomial *left, Polynomial *right);
  char *getConstraintPretty(PPL_Manager *manager);
#ifdef __cplusplus
}
#endif

#endif
