//===-- Reorder.cpp ----------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <cmath>
#include <immintrin.h>

#include "klee/Reorder.h"
#include "klee/Expr.h"

using namespace std;
using namespace llvm;
using namespace klee;

void Reorder::getMultBounds(const vector<ref<Expr> > &ops, vector<MultRes> &res){
  vector<APFloat> opAF;
  for(int i = 0; i < ops.size(); i ++){
    ref<ConstantExpr> c = dyn_cast<ConstantExpr>(ops[i]);
    opAF.push_back(c->getAPFValue());
  }

  res.push_back(getMultBound(0, Mult, opAF));
  res.push_back(getMultBound(1, Mult, opAF));
}

void Reorder::getDotBounds(const vector<pair<ref<Expr>, ref<Expr> > > &ops, 
			   vector<ref<Expr> > &res){
  vector<APFloat> opsl;
  vector<APFloat> opsr;

  for(int i = 0; i < ops.size(); i ++){
    ref<ConstantExpr> l = dyn_cast<ConstantExpr>(ops[i].first);
    ref<ConstantExpr> r = dyn_cast<ConstantExpr>(ops[i].second);
    opsl.push_back(l->getAPFValue());
    opsr.push_back(r->getAPFValue());
  }

  APFloat min = getDotBound(0, opsl, opsr, res);
  APFloat max = getDotBound(1, opsl, opsr, res);

  ref<ConstantExpr> ceMin = ConstantExpr::alloc(min);
  ref<ConstantExpr> ceMax = ConstantExpr::alloc(max);
  res.push_back(ceMin);
  res.push_back(ceMax);
}


MultRes Reorder::getMultBound(int direction, const vector<APFloat> &ops){
    APFloat c, t;
    std::vector<std::vector<APFloat> > values;
    std::vector<std::vector<int> > choices;
    int len = ops.size();
    int origRound = fegetround();

    values.resize(len);
    for(int i = 0; i < len; i ++){
      values[i].resize(len);
    }
    choices.resize(len);
    for(int i = 0; i < len; i ++){
      choices[i].resize(len);
    }

    //fesetround(roundMode);
    for(int i = 0; i < len; i ++){
      values[i][i] = ops[i];
    }
  
    for(int l = 2; l <= len; l ++){
      for(int i = 0; i < len - l + 1; i ++){
	int j = i + l - 1;
	choices[i][j] = i;
	c = getCost(values[i][i], values[i+1][j], op);
	values[i][j] = getValue(values[i][i], values[i+1][j], op);	
	for(int k = i + 1;  k < j; k++){
	  t = getCost(values[i][k], values[k+1][j], op);
	  if(direction == 1){
	    if(t.compare(c) == APFloat::cmpGreaterThan){
	      c = t;
	      choices[i][j] = k;
	      values[i][j] = getValue(values[i][k], values[k+1][j], op);
	    }
	  }else{
	    if(t.compare(c) == APFloat::cmpLessThan){
	      c = t;
	      choices[i][j] = k;	    
	      values[i][j] = getValue(values[i][k], values[k+1][j], op);
	    }
	  }
	}     
      }    
    }
    //fesetround(origRound);

    ref<ConstantExpr> ce = ConstantExpr::alloc(values[0][len-1]);
    ref<ConstantExpr> op1 = ConstantExpr::alloc(values[0][choices[0][len-1] ]);
    ref<ConstantExpr> op2 = ConstantExpr::alloc(values[choices[0][len - 1] + 1][len - 1]);
    MultRes mr;
    mr.res = ce;
    mr.op1 = op1;
    mr.op2 = op2;
    return mr;
  }
 
APFloat Reorder::getDotBound(int direction, const vector<APFloat> &opsl, 
			     const vector<APFloat> &opsr){
    std::vector<std::vector<APFloat> > values;
    std::vector<std::vector<int> > choices;
    int len = opsl.size();
    int origRound = fegetround();

    values.resize(len);
    for(int i = 0; i < len; i ++){
      values[i].resize(len);
    }
    choices.resize(len);
    for(int i = 0; i < len; i ++){
      choices[i].resize(len);
    }

    //fesetround(roundMode);

    for(int i = 0; i < len - 1; i ++){
      APFloat t1 = opsl[i+1] * opsr[i+1];
      APFloat t2 = opsl[i] * opsr[i];
      APFloat r1, r2;
      if(opsl[i].getSemantics() == &APFloat::IEEEsingle){
	__m128 a, b, c, r;
	a[0] = opsl[i].convertToFloat();
	a[1] = opsl[i+1].convertToFloat();
	b[0] = opsr[i].convertToFloat();
	b[1] = opsr[i+1].convertToFloat();
	c[0] = t1.convertToFloat();
	c[1] = t2.convertToFloat();
	r = _mm_fmadd_ps(a, b, c);
	r1 = APFloat(r[0]);
	r2 = APFloat(r[1]);
      }else{
	__m128d a, b, c, r;
	a[0] = opsl[i].convertToDouble();
	a[1] = opsl[i+1].convertToDouble();
	b[0] = opsr[i].convertToDouble();
	b[1] = opsr[i+1].convertToDouble();
	c[0] = t1.convertToDouble();
	c[1] = t2.convertToDouble();
	r = _mm_fmadd_pd(a, b, c);
	r1 = APFloat(r[0]);
	r2 = APFloat(r[1]);
      }

      if(direction == 1){
	if(r1.compare(r2) == APFloat::cmpGreaterThan){
	  values[i][i+1] = r1;
	  choices[i][i+1] = i;
	}else{
	  values[i][i+1] = r2;
	  choices[i][i+1] = i + 1;
	}
      }else{
	if(r1.compare(r2) == APFloat::cmpLessThan){
	  values[i][i+1] = r1;
	  choices[i][i+1] = i;
	}else{
	  values[i][i+1] = r2;
	  choices[i][i+1] = i + 1;
	}
      }
    }
    for(int l = 3; l <= len; l ++){
      for(int i = 0; i < len - l + 1; i ++){
	int j = i + l - 1;
	APFloat r1, r2;
	if(opsl[i].getSemantics() == &APFloat::IEEEsingle){
	  __m128 a, b, c, r;
	  a[0] = opsl[i].convertToFloat();
	  a[1] = opsl[j].convertToFloat();
	  b[0] = opsr[i].convertToFloat();
	  b[1] = opsr[j].convertToFloat();
	  c[0] = values[i+1][j].convertToFloat();
	  c[1] = values[i][j-1].convertToFloat();
	  r = _mm_fmadd_ps(a, b, c);
	  r1 = APFloat(r[0]);
	  r2 = APFloat(r[1]);
	}else{ // for the double type
	  __m128d a, b, c, r;
	  a[0] = opsl[i].convertToDouble();
	  a[1] = opsl[j].convertToDouble();
	  b[0] = opsr[i].convertToDouble();
	  b[1] = opsr[j].convertToDouble();
	  c[0] = values[i+1][j].convertToDouble();
	  c[1] = values[i][j-1].convertToDouble();
	  r = _mm_fmadd_pd(a, b, c);
	  r1 = APFloat(r[0]);
	  r2 = APFloat(r[1]);
	}
	if(direction == 1){
	  if(r1.compare(r2) == APFloat::cmpGreaterThan){
	    values[i][j] = r1;
	    choices[i][j] = i;
	  }else{
	    values[i][j] = r2;
	    choices[i][j] = j - 1;
	  }
	}else{
	  if(r1.compare(r2) == APFloat::cmpLessThan){
	    values[i][j] = r1;
	    choices[i][j] = i;
	  }else{
	    values[i][j] = r2;
	    choices[i][j] = j - 1;
	  }
	}

	for(int k = i + 1; k < j - 1; k ++){
	  T t = values[i][k] + values[k+1][j];
	  if(direction == 1){
	    if(t.compare(values[i][j]) == APFloat::cmpGreaterThan){
	      values[i][j] = t;
	      choices[i][j] = k;
	    }
	  }else{
	    if(t.compare(values[i][j]) == APFloat::cmpLessThan){
	      values[i][j] = t;
	      choices[i][j] = k;
	    }
	  }
	}
      }    
    }
    //fesetround(origRound);
    return values[0][len - 1];
  } 

  APFloat Reorder::getCost(APFloat a, APFloat b){
    APFloat c;
    if(roundMode == FE_TONEAREST || roundMode == FE_TOWARDZERO){
      c = a.clearSign() * b.clearSign();
    }
      /*
      else if(roundMode == FE_DOWNWARD){
	fesetround(FE_UPWARD);
	c = a.clearSign() * b.clearSign();
	fesetround(FE_DOWNWARD);
      }else{
	fesetround(FE_DOWNWARD);
	c = fabs(a) * fabs(b);
	fesetround(FE_UPWARD);
	}*/   
    return c;
  }
}
