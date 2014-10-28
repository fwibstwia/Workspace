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
#include "klee/Expr.h"

#include <vector>
#include <z3++.h>

namespace klee {
  class Z3ArrayExprHash : public ArrayExprHash<z3::expr*> {
    
    friend class Z3Builder;
    
  public:
    Z3ArrayExprHash() {};
    virtual ~Z3ArrayExprHash();
  };

  class Z3Builder {
  private:  
    /// optimizeDivides - Rewrite division and reminders by constants
    /// into multiplies and shifts. Z3 should probably handle this for
    /// use.
    z3::context *c;
    bool optimizeDivides;
    Z3ArrayExprHash _arr_hash;

  private:
    z3::expr getInitialArray(const Array *root);
    z3::expr getArrayForUpdate(const Array *root, 
				      const UpdateNode *un);
  public:
    Z3Builder(z3::context *_c, bool _optimizeDivides=false): c(_c), 
							    optimizeDivides(_optimizeDivides){
    }

    ref<Expr> getInitialRead(const Array *os);

    z3::expr construct(ref<Expr> e);
    z3::expr constructDeclaration(const Array *object);
  };

}
#endif
