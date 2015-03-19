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
  vector<float> opAF;
  for(int i = 0; i < ops.size(); i ++){
    ref<ConstantExpr> c = dyn_cast<ConstantExpr>(ops[i]);
    opAF.push_back(c->getAPFValue().convertToFloat());
  }

  res.push_back(getMultBound(0, opAF));
  res.push_back(getMultBound(1, opAF));
}

void Reorder::getDotBounds(const vector<pair<ref<Expr>, ref<Expr> > > &ops, 
			   vector<ref<Expr> > &res){
  vector<float> opsl;
  vector<float> opsr;

  for(int i = 0; i < ops.size(); i ++){
    ref<ConstantExpr> l = dyn_cast<ConstantExpr>(ops[i].first);
    ref<ConstantExpr> r = dyn_cast<ConstantExpr>(ops[i].second);
    opsl.push_back(l->getAPFValue().convertToFloat());
    opsr.push_back(r->getAPFValue().convertToFloat());
  }

  float min = getDotBound(0, opsl, opsr);
  float max = getDotBound(1, opsl, opsr);
  APFloat apMin(min);
  APFloat apMax(max);

  ref<ConstantExpr> ceMin = ConstantExpr::alloc(apMin);
  ref<ConstantExpr> ceMax = ConstantExpr::alloc(apMax);
  res.push_back(ceMin);
  res.push_back(ceMax);
}


MultRes Reorder::getMultBound(int direction, const vector<float> &ops){
    float c, t;
    std::vector<std::vector<float> > values;
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
	c = getCost(values[i][i], values[i+1][j]);
	values[i][j] = values[i][i] * values[i+1][j];	
	for(int k = i + 1;  k < j; k++){
	  t = getCost(values[i][k], values[k+1][j]);
	  if(direction == 1){
	    if(t > c){
	      c = t;
	      choices[i][j] = k;
	      values[i][j] = values[i][k] * values[k+1][j];
	    }
	  }else{
	    if(t < c){
	      c = t;
	      choices[i][j] = k;	    
	      values[i][j] = values[i][k] * values[k+1][j];
	    }
	  }
	}     
      }    
    }

    fesetround(origRound);

    APFloat ceAP(values[0][len-1]);
    APFloat op1AP(values[0][choices[0][len-1] ]);
    APFloat op2AP(values[choices[0][len - 1] + 1][len - 1]);
    ref<ConstantExpr> ce = ConstantExpr::alloc(ceAP);
    ref<ConstantExpr> op1 = ConstantExpr::alloc(op1AP);
    ref<ConstantExpr> op2 = ConstantExpr::alloc(op2AP);
    MultRes mr;
    mr.res = ce;
    mr.op1 = op1;
    mr.op2 = op2;
    return mr;
}
 
float Reorder::getDotBound(int direction, const vector<float> &opsl, 
			     const vector<float> &opsr){
 std::vector<std::vector<float> > values;
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
   float t1 = opsl[i+1] * opsr[i+1];
   float t2 = opsl[i] * opsr[i];
   float r1, r2;
 
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
     float r1, r2;

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
       float t = values[i][k] + values[k+1][j];
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

float Reorder::getCost(float a, float b){
  float c;
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
