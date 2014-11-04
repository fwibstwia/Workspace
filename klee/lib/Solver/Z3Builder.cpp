#include "Z3Builder.h"

#include "klee/Expr.h"
#include "klee/Solver.h"
#include "SolverStats.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <sstream>
#include <vector>

using namespace z3;
using namespace klee;

Z3ArrayExprHash::~Z3ArrayExprHash(){
}

Z3Builder::~Z3Builder(){
  ///fix me: delete _arr_hash;
}

void Z3Builder::getInitialRead(const Array *os, model &m, std::vector<unsigned char> &value){  
  expr res = m.get_const_interp(getInitialArray(os).decl());
  if(res.is_numeral()){
    Z3_string s = Z3_get_numeral_decimal_string(*c, res, 50);
  
    float v = strtof(s, NULL);
    char *p = reinterpret_cast<char*>(&v);
    std::copy(p, p + sizeof(float), value.begin());
  } 
}

expr Z3Builder::getInitialArray(const Array *root){
  assert(root);
  expr *array_expr;
  bool hashed = _arr_hash.lookupArrayExpr(root, array_expr);
  if(!hashed){
    if(root-> getDomain() != Expr::InvalidWidth){
      sort I = c -> int_sort();
      sort R = c -> real_sort();
      sort A = c -> array_sort(I, R);
      array_expr = new expr(c -> constant(c -> str_symbol((root -> name).c_str()), A));
    } else {
      array_expr = new expr(c -> real_const((root -> name).c_str()));
    }
    _arr_hash.hashArrayExpr(root, array_expr);
  }
  return *array_expr;
}

expr Z3Builder::getArrayForUpdate(const Array *root, 
                                       const UpdateNode *un) {
  expr *un_expr;
  if (!un) {
    return getInitialArray(root);
  }
  else {
      // FIXME: This really needs to be non-recursive.
    if(root -> getDomain() != Expr::InvalidWidth){
      bool hashed = _arr_hash.lookupUpdateNodeExpr(un, un_expr);
      
      if (!hashed) {
	un_expr = new expr(store(getArrayForUpdate(root, un->next), construct(un->index), construct(un->value)));
       _arr_hash.hashUpdateNodeExpr(un, un_expr);
      }
      return(*un_expr);
    }
    else{
      bool hashed = _arr_hash.lookupUpdateNodeExpr(un, un_expr);
      if(!hashed){
	un_expr = new expr(construct(un->value));
	_arr_hash.hashUpdateNodeExpr(un, un_expr);
      }
      return *un_expr;
    }
  }
}

expr Z3Builder::construct(ref<Expr> e){

  ++stats::queryConstructs;

  switch (e->getKind()){

  case Expr::Constant: {
    ConstantExpr *CE = cast<ConstantExpr>(e);
    return c->real_val((__uint64)CE->getZExtValue());
  }

  case Expr::NotOptimized: {
    NotOptimizedExpr *noe = cast<NotOptimizedExpr>(e);
    return construct(noe->src);
  }
  
  case Expr::Reorder:{
    ReorderExpr *re = cast<ReorderExpr>(e);
    return construct(re->src);
  }

  case Expr::Read: {
    ReadExpr *re = cast<ReadExpr>(e);
    assert(re && re->updates.root);
    if((re->updates.root)-> getDomain() != Expr::InvalidWidth){
      return select(getArrayForUpdate(re->updates.root, re->updates.head), construct(re->index));
    }
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
    SubExpr *ae = cast<SubExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left - right;
  }

  case Expr::Mul: {
    MulExpr *ae = cast<MulExpr>(e);
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
    FAddExpr *ae = cast<FAddExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left + right;
  }

  case Expr::FSub: {
    FSubExpr *ae = cast<FSubExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left - right;
  }

  case Expr::FMul: {
    FMulExpr *ae = cast<FMulExpr>(e);
    expr left = construct(ae->left);
    expr right = construct(ae->right);
    return left * right;
  }
  case Expr::FDiv: {
    FDivExpr *ae = cast<FDivExpr>(e);
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
    OrExpr *ae = cast<OrExpr>(e);
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
    if(ConstantExpr *CE = dyn_cast<ConstantExpr>(ee->left)){
      if(CE->isTrue())
	return right;
      if(CE->isFalse())
	return !right;
    }
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
