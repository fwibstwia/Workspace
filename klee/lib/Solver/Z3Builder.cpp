#include "Z3Builder.h"

#include "klee/Expr.h"
#include "klee/Solver.h"
#include "SolverStats.h"

#include <cstdio>
#include <cassert>
#include <sstream>
#include <vector>

using namespace z3;
using namespace klee;

ref<Expr> getInitialRead(const Array *os){

}

expr Z3Builder::getInitialArray(const Array *root) {
  
  assert(root);
  expr array_expr;
  bool hashed = _arr_hash.lookupArrayExpr(root, array_expr);
  
  if (!hashed) {
    // STP uniques arrays by name, so we make sure the name is unique by
    // including the address.
    char buf[32];
    unsigned const addrlen = sprintf(buf, "_%p", (const void*)root) + 1; // +1 for null-termination
    unsigned const space = (root->name.length() > 32 - addrlen)?(32 - addrlen):root->name.length();
    memmove(buf + space, buf, addrlen); // moving the address part to the end
    memcpy(buf, root->name.c_str(), space); // filling out the name part
    
    array_expr = buildArray(buf, root->getDomain(), root->getRange());
    
    if (root->isConstantArray()) {
      // FIXME: Flush the concrete values into STP. Ideally we would do this
      // using assertions, which is much faster, but we need to fix the caching
      // to work correctly in that case.
      for (unsigned i = 0, e = root->size; i != e; ++i) {
	::VCExpr prev = array_expr;
	array_expr = vc_writeExpr(vc, prev,
                       construct(ConstantExpr::alloc(i, root->getDomain()), 0),
                       construct(root->constantValues[i], 0));
	vc_DeleteExpr(prev);
      }
    }
    
    _arr_hash.hashArrayExpr(root, array_expr);
  }
  
  return(array_expr); 
}

expr Z3Builder::getArrayForUpdate(const Array *root, 
                                       const UpdateNode *un) {
  if (!un) {
      return(getInitialArray(root));
  }
  else {
      // FIXME: This really needs to be non-recursive.
      ::VCExpr un_expr;
      bool hashed = _arr_hash.lookupUpdateNodeExpr(un, un_expr);
      
      if (!hashed) {
	un_expr = vc_writeExpr(vc,
                               getArrayForUpdate(root, un->next),
                               construct(un->index, 0),
                               construct(un->value, 0));
	
	_arr_hash.hashUpdateNodeExpr(un, un_expr);
      }
      
      return(un_expr);
  }
}

expr Z3Builder::construct(ref<Expr> e){

  ++stats::queryConstructs;

  switch (e->getKind()){

  case Expr::Constant: {
    ConstantExpr *CE = cast<ConstantExpr>(e);
    return c->real_val(CE->getZExtValue());
  }

  case Expr::NotOptimized: {
    NotOptimizedExpr *noe = cast<NotOptimizedExpr>(e);
    return construct(noe->src);
  }

  case Expr::Read: {
    ReadExpr *re = cast<ReadExpr>(e);
    assert(re && re->updates.root);
    if(
  }

  case Expr::Select: {
    SelectExpr *se = cast<SelectExpr>(e);
    expr cond = construct(se->cond);
    expr tExpr = construct(se->trueExpr);
    expr fExpr = construct(se->falseExpr);
    return ite(cond, tExpr, fExpr);
  }

    //Arithmetic

  case Expr::Add: {
    AddExpr *ae = cast<AddExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left + right;
  }

  case Expr::Sub: {
    AddExpr *ae = cast<AddExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left - right;
  }

  case Expr::Mul: {
    AddExpr *ae = cast<AddExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left * right;
  }

  case Expr::UDiv: {
  }

  case Expr::SDiv: {
  }

  case Expr::URem: {
  }

  case Expr::SRem: {
  }

  case Expr::FAdd: {
    AddExpr *ae = cast<AddExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left + right;
  }

  case Expr::Fsub: {
    AddExpr *ae = cast<AddExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left - right;
  }

  case Expr::FMul: {
    AddExpr *ae = cast<AddExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left * right;
  }
  case Expr::FDiv: {
    AddExpr *ae = cast<AddExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left / right;
  }

  case Expr::FRem: {
  }
    //logic
  case Expr::Not: {
    NotExpr *ne = cast<NotExpr>(e);
    expr e = construct(ne->expr);
    return !e;
  }
    
  case Expr::And: {
    AndExpr *ae = cast<AndExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left && right;
  }

  case Expr::Or: {
    AndExpr *ae = cast<AndExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left || right;
  }

  case Expr::Xor: {
  }

    //comparison
  case Expr::Eq: {
    EqExpr *ee = cast<EqExpr>(e);
    expr left = construct(ee->left);
    expr right = construct(ee->right);
    return left == right;
  }

  case Expr::Ult: {
  }

  case Expr::Ule: {
  }

  case Expr::slt: {
  }

  case Expr::sle: {
  }
  
  case Expr::FOlt: {
    FOltExpr *ee = cast<FOltExpr>(e);
    expr left = construct(ee->left);
    expr right = construct(ee->right);
    return left < right;
  }

#if 0
  case Expr::Ne:
  case Expr::Ugt:
  case Expr::Uge:
  case Expr::Sgt:
  case Expr::Sge:
  case Expr::FOgt;
#endif

  default:
    assert(0 && "unhandled Expr type");
    return c->bool_val(true);
  }
}
