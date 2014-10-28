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

expr Z3Builder::constructDeclaration(const Array* object){
  expr *array_expr;
  if(object-> getDomain() != Expr::InvalidWidth){
    sort I = c -> int_sort();
    sort R = c -> real_sort();
    sort A = c -> array_sort(I, R);
    array_expr = new expr(c -> constant(c -> str_symbol((object -> name).c_str()), A));
  } else {
    array_expr = new expr(c -> real_val((object -> name).c_str()));
  }
  _arr_hash.hashArrayExpr(object, array_expr);
  return *array_expr;
}

expr Z3Builder::getArrayForUpdate(const Array *root, 
                                       const UpdateNode *un) {
  expr *un_expr;
  if (!un) {
    _arr_hash.lookupArrayExpr(root, un_expr);
    return *un_expr;
  }
  else {
      // FIXME: This really needs to be non-recursive.
    if(root -> getDomain() != Expr::InvalidWidth){
      //      bool hashed = _arr_hash.lookupUpdateNodeExpr(un, un_expr);
      
      //      if (!hashed) {
      //	_arr_hash.hashUpdateNodeExpr(un, un_expr);
      //      }
      //      return(*un_expr);
    }
    else{
      
    }
  }
}

expr Z3Builder::construct(ref<Expr> e){

  ++stats::queryConstructs;

  switch (e->getKind()){

  case Expr::Constant: {
    ConstantExpr *CE = cast<ConstantExpr>(e);
    return c->real_val((__int64)CE->getZExtValue());
  }

  case Expr::NotOptimized: {
    NotOptimizedExpr *noe = cast<NotOptimizedExpr>(e);
    return construct(noe->src);
  }

  case Expr::Read: {
    ReadExpr *re = cast<ReadExpr>(e);
    assert(re && re->updates.root);
    return getArrayForUpdate(re->updates.root, re->updates.head);
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

  case Expr::FSub: {
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

  case Expr::Slt: {
  }

  case Expr::Sle: {
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
