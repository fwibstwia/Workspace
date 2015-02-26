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
#include <cmath>
#include <immintrin.h>

#include "llvm/ATD/APFloat.h"

using namespace std;
using namespace llvm;

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

    APFloat getPlusMax(const std::vector<APFloat> &ops){
      return getBound(1, Plus, ops);
    }

    APFloat getPlusMin(const std::vector<APFloat> &ops){
      return getBound(0, Plus, ops);
    }

    APFloat getMultMax(const std::vector<APFloat> &ops){
      return getBound(1, Mult, ops);
    }

    APFloat getMultMin(const std::vector<APFloat> &ops){
      return getBound(0, Mult, ops);
    }

    APFloat getFMAMax(const std::vector<APFloat> &opsl, const std::vector<APFloat> &opsr){
      return getFMABound(1, opsl, opsr);
    }

    APFloat getFMAMin(const std::vector<APFloat> &opsl, const std::vector<APFloat> &opsr){
      return getFMABound(0, opsl, opsr);
    }

    APFloat getFMAExp(const APFloat &mult0, const APFloat &mult1, const APFloat &addend);

  private:
    APFloat getCost(APFloat a, APFloat b, Operation op);
    APFloat getValue(const APFloat &a, const APFloat &b, Operation op);
    APFloat getBound(int direction, Operation op, const std::vector<APFloat> &ops);
    APFloat getFMABound(int direction, const std::vector<APFloat> &opsl, 
			const std::vector<APFloat> &opsr);

  private:
    int roundMode;
  };

  APFloat Reorder::getFMAExp(const APFloat &mult0, const APFloat &mult1, 
				      const APFloat &addend){
     if(mult0.getSemantics() == &APFloat::IEEEsingle){
	__m128 a, b, c, r;
	a[0] = mult0.convertToFloat();
	b[0] = mult1.convertToFloat();
	c[0] = addend.convertToFloat();
	r = _mm_fmadd_ps(a, b, c);
	return APFloat(r[0]);
     }
     __m128d a, b, c, r;
     a[0] = mult0.convertToDouble();
     b[0] = mult1.convertToDouble();
     c[0] = addend.convertToDouble();
     r = _mm_fmadd_pd(a, b, c);
     return APFloat(r[0]);
  }

  APFloat Reorder::getBound(int direction, Operation op, const vector<APFloat> &ops){
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
    return values[0][len - 1];
  }
 
  APFloat Reorder::getFMABound(int direction, const std::vector<APFloat> &opsl, const std::vector<APFloat> &opsr){
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

  APFloat Reorder::getCost(APFloat a, APFloat b, Operation op){
    APFloat c;
    if(op == Plus){
      return a + b;
    }

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

  APFloat Reorder::getValue(const APFloat &a, const APFloat &b, Operation op){
    if(op == Plus){
      return a + b;
    }else{
      return a * b;
    }
  }
}
#endif
