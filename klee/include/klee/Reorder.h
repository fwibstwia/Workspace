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
#include <cfenv>

namespace klee {
  template<typename T>
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

    T getPlusMax(const std::vector<T> &ops){
      return getBound(1, Plus, ops);
    }

    T getPlusMin(const std::vector<T> &ops){
      return getBound(0, Plus, ops);
    }

    T getMultMax(const std::vector<T> &ops){
      return getBound(1, Mult, ops);
    }

    T getMultMin(const std::vector<T> &ops){
      return getBound(0, Mult, ops);
    }
  private:
    T getCost(T a, T b, Operation op);
    T getValue(T a, T b, Operation op);
    T getBound(int direction, Operation op, const std::vector<T> &ops);

  private:
    int roundMode;
  };
}
#endif
