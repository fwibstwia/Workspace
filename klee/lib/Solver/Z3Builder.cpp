#include "Z3Builder.h"

#include "klee/Expr.h"
#include "klee/Solver.h"
#include "SolverStats.h"

#include <cstdio>
#include <cassert>
#include <sstream>
#include <vector>

expr Z3Builder::construct(ref<Expr> e){

  ++stats::queryConstructs;
  switch (e->getKind()){
  case Expr::Constant:{
    ConstantExpr *CE = cast<ConstantExpr>(e);
  }
  }
}
