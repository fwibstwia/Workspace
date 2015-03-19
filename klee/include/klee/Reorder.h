//===-- Reorder.h --------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_REORDER_H_
#define KLEE_REORDER_H_

#include <vector>
#include <utility>
#include <fenv.h>
#include <cmath>
#include <immintrin.h>

#include "klee/Expr.h"

using namespace std;
using namespace llvm;

namespace klee {
  class MultRes{
  public:
    ref<Expr> res;
    ref<Expr> op1;
    ref<Expr> op2;
  };

  class Reorder {    
  public:
    Reorder(int _roundMode):roundMode(_roundMode){}

    int getRoundMode(){return roundMode;}

    void getMultBounds(const vector<ref<Expr> > &ops, std::vector<MultRes> &res);
    
    void getDotBounds(const vector<pair<ref<Expr>, ref<Expr> > > &ops,
		      std::vector<ref<Expr> > &res);
  private:
    MultRes getMultBound(int direction, const std::vector<float> &ops);
    float getDotBound(int direction, const std::vector<float> &opsl, 
			const std::vector<float> &opsr);
    float getCost(float a, float b);

  private:
    int roundMode;
  };
}
#endif
