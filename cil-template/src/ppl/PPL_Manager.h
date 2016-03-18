#ifndef PPL_MANAGER
#define PPL_MANAGER

#include <vector>
#include <ppl.hh>

#include "C_Expr_defs.h"

using namespace std;
using namespace Parma_Polyhedra_Library;

class PPL_Manager {
 public:
 PPL_Manager(int *varIdArray, int len): dimLen(len), power_poly(len, EMPTY) {

    for(int i = 0; i < dimLen; i ++){
      varIdMap[varIdArray[i]] = new Variable(i);
    }
    NNC_Polyhedron p(len, UNIVERSE);
    //add constraints
    p.add_constraint((*varIdMap[varIdArray[0]]) >= 1);
    p.add_constraint(10*(*varIdMap[varIdArray[0]]) <= 20);

    p.add_constraint((*varIdMap[varIdArray[1]]) >= 1);
    p.add_constraint(10*(*varIdMap[varIdArray[1]]) <= 20);

    
    p.add_constraint((*varIdMap[varIdArray[2]]) >= 1);
    p.add_constraint(10*(*varIdMap[varIdArray[2]]) <= 20);

    
    p.add_constraint((*varIdMap[varIdArray[3]]) >= 1);
    p.add_constraint(10*(*varIdMap[varIdArray[3]]) <= 20);

    p.add_constraint((*varIdMap[varIdArray[4]]) >= 1);
    p.add_constraint(10*(*varIdMap[varIdArray[4]]) <= 20);
    p.add_constraint((*varIdMap[varIdArray[5]]) >= 1);
    p.add_constraint(10*(*varIdMap[varIdArray[5]]) <= 20);
    p.add_constraint((*varIdMap[varIdArray[6]]) >= 1);
    p.add_constraint(10*(*varIdMap[varIdArray[6]]) <= 20);
    
    //p.add_constraint(*varIdMap[varIdArray[3]] >= -1);
    //p.add_constraint(*varIdMap[varIdArray[3]] <= 2);
    
    power_poly.add_disjunct(p);

  }

 PPL_Manager(const PPL_Manager &manager): dimLen(manager.dimLen),
    power_poly(manager.power_poly), varIdMap(manager.varIdMap){    
  }

  map<int, Variable*> varIdMap; //map vid to PPL Variable Type
  FP_Linear_Form_Abstract_Store lf_abstract_store;
  Pointset_Powerset<NNC_Polyhedron> power_poly;
  int dimLen;
};
#endif
