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

#include "llvm/ATD/APFloat.h"

using namespace std;
using namespace llvm;

namespace klee {
  class Reorder {
  public:
    class MultRes{
    public:
      ref<Expr> res;
      ref<Expr> op1;
      ref<Expr> op2;
    }
    
  public:
    Reorder(int _roundMode):roundMode(_roundMode){}

    int getRoundMode(){return roundMode;}

    void getMultBounds(const vector<ref<Expr> > &ops, std::vector<MultRes> &res);
    
    void getDotBounds(const vector<pair<ref<Expr>, ref<Expr> > > ops,
		      std::vector<ref<Expr> > &res);
  private:
    MultRes getMultBound(int direction, const std::vector<APFloat> &ops);
    APFloat getFMABound(int direction, const std::vector<APFloat> &opsl, 
			const std::vector<APFloat> &opsr);
    APFloat getCost(APFloat a, APFloat b);

  private:
    int roundMode;
  };
#endif
