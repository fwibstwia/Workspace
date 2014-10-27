//===-- STPBuilder.h --------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef __UTIL_Z3BUILDER_H__
#define __UTIL_Z3BUILDER_H__

#include "klee/util/ExprHashMap.h"
#include "klee/util/ArrayExprHash.h"
#include "klee/Config/config.h"

#include <vector>
#include"z3++.h"

#endif

namespace klee {

class Z3Builder {
private:  
  /// optimizeDivides - Rewrite division and reminders by constants
  /// into multiplies and shifts. STP should probably handle this for
  /// use.
  bool optimizeDivides;

public:
  Z3Builder(bool _optimizeDivides=true);

  expr getTrue();
  expr getFalse();
  expr getTempVar(Expr::Width w);
  expr getInitialRead(const Array *os, unsigned index);

  expr construct(ref<Expr> e) { 
    expr res = construct(e, 0);
    constructed.clear();
    return res;
  }
};

}

#endif
