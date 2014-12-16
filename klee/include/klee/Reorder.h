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

using namespace std;

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

    T getFMAMax(const std::vector<T> &opsl, const std::vector<T> &opsr){
      return getFMABound(1, opsl, opsr);
    }

    T getFMAMin(const std::vector<T> &opsl, const std::vector<T> &opsr){
      return getFMABound(0, opsl, opsr);
    }

  private:
    T getCost(T a, T b, Operation op);
    T getValue(T a, T b, Operation op);
    T getBound(int direction, Operation op, const std::vector<T> &ops);
    T getFMABound(int direction, const std::vector<T> &opsl, const std::vector<T> &opsr);

  private:
    int roundMode;
  };

  template <typename T>  
  T Reorder<T>::getBound(int direction, Operation op, const vector<T> &ops){
    T c, t;
    std::vector<std::vector<T> > values;
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

    fesetround(roundMode);
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
	    if(t > c){
	      c = t;
	      choices[i][j] = k;
	      values[i][j] = getValue(values[i][k], values[k+1][j], op);
	    }
	  }else{
	    if(t < c){
	      c = t;
	      choices[i][j] = k;	    
	      values[i][j] = getValue(values[i][k], values[k+1][j], op);
	    }
	  }
	}     
      }    
    }

    fesetround(origRound);
    return values[0][len - 1];
  }

  template <typename T>  
  T Reorder<T>::getFMABound(int direction, const std::vector<T> &opsl, const std::vector<T> &opsr){
    std::vector<std::vector<T> > values;
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

    fesetround(roundMode);

    for(int i = 0; i < len - 1; i ++){
      T t1 = opsl[i+1] * opsr[i+1];
      T t2 = opsl[i] * opsr[i];
      T r1, r2;
      if(sizeof(T) == 32){
	__m128 a, b, c, r;
	a[0] = opsl[i];
	a[1] = opsl[i+1];
	b[0] = opsr[i];
	b[1] = opsr[i+1];
	c[0] = t1;
	c[1] = t2;
	r = _mm_fmadd_ps(a, b, c);
	r1 = r[0];
	r2 = r[1];
      }else{
	__m128d a, b, c, r;
	a[0] = opsl[i];
	a[1] = opsl[i+1];
	b[0] = opsr[i];
	b[1] = opsr[i+1];
	c[0] = t1;
	c[1] = t2;
	r = _mm_fmadd_pd(a, b, c);
	r1 = r[0];
	r2 = r[1];
      }

      if(direction == 1){
	if(r1 > r2){
	  values[i][i+1] = r1;
	  choices[i][i+1] = i;
	}else{
	  values[i][i+1] = r2;
	  choices[i][i+1] = i + 1;
	}
      }else{
	if(r1 < r2){
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
	T r1, r2;
	if(sizeof(T) == 32){
	  __m128 a, b, c, r;
	  a[0] = opsl[i];
	  a[1] = opsl[j];
	  b[0] = opsr[i];
	  b[1] = opsr[j];
	  c[0] = values[i+1][j];
	  c[1] = values[i][j-1];
	  r = _mm_fmadd_ps(a, b, c);
	  r1 = r[0];
	  r2 = r[1];
	}else{ // for the double type
	  __m128d a, b, c, r;
	  a[0] = opsl[i];
	  a[1] = opsl[j];
	  b[0] = opsr[i];
	  b[1] = opsr[j];
	  c[0] = values[i+1][j];
	  c[1] = values[i][j-1];
	  r = _mm_fmadd_pd(a, b, c);
	  r1 = r[0];
	  r2 = r[1];
	}
	if(direction == 1){
	  if(r1 > r2){
	    values[i][j] = r1;
	    choices[i][j] = i;
	  }else{
	    values[i][j] = r2;
	    choices[i][j] = j - 1;
	  }
	}else{
	  if(r1 < r2){
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
	    if(t > values[i][j]){
	      values[i][j] = t;
	      choices[i][j] = k;
	    }
	  }else{
	    if(t < values[i][j]){
	      values[i][j] = t;
	      choices[i][j] = k;
	    }
	  }
	}
      }    
    }
    fesetround(origRound);
    return values[0][len - 1];
  } 

  template <typename T>  
  T Reorder<T>::getCost(T a, T b, Operation op){
    T c;
    if(op == Plus){
      return a + b;
    }
    if(a * b >= 0){
      c = a * b;
    } else{
      if(roundMode == FE_TONEAREST || roundMode == FE_TOWARDZERO){
	c = fabs(a) * fabs(b);
      }else if(roundMode == FE_DOWNWARD){
	fesetround(FE_UPWARD);
	c = fabs(a) * fabs(b);
	fesetround(FE_DOWNWARD);
      }else{
	fesetround(FE_DOWNWARD);
	c = fabs(a) * fabs(b);
	fesetround(FE_UPWARD);
      }
    }
    return c;
  }

  template <typename T>  
  T Reorder<T>::getValue(T a, T b, Operation op){
    if(op == Plus){
      return a + b;
    }else{
      return a * b;
    }
  }
}
#endif
