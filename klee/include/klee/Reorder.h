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
#include <fenv.h>

namespace klee {
  class Reorder {
  public:
    enum Operation{
      Plus,
      Mult,
      Fma
    };
  public:
    Reorder(int _roundMode):roundMode(_roundMode){}

    int getRoundMode(){return roundMode;}

    float getPlusMax(const std::vector<float> &ops){
      return getBound(1, Plus, ops);
    }

    float getPlusMin(const std::vector<float> &ops){
      return getBound(0, Plus, ops);
    }

    float getMultMax(const std::vector<float> &ops){
      return getBound(1, Mult, ops);
    }

    float getMultMin(const std::vector<float> &ops){
      return getBound(0, Mult, ops);
    }
     
    float getFMAMax(const std::vector<float> &opsl, const std::vector<float> &opsr){
      return getFMABound(1, opsl, opsr);
    }

    float getFMAMin(const std::vector<float> &opsl, const std::vector<float> &opsr){
      return getFMABound(0, opsl, opsr);
    }

  private:
    float getCost(float a, float b, Operation op);
    float getValue(float a, float b, Operation op);
    float getBound(int direction, Operation op, const std::vector<float> &ops);
    float getFMABound(int direction, 
		      const std::vector<float> &opsl, 
		      const std::vector<float> &opsr);
  private:
    int roundMode;
  };
}
#endif
