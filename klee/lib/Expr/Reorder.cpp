//===-- Reorder.cpp ----------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "klee/Reorder.h"
#include <cmath>

using namespace std;
using namespace klee;
  
float Reorder::getBound(int direction, Operation op, const vector<float> &ops){
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


float Reorder::getFMABound(int direction, const std::vector<float> &opsl, const std::vector<float> &opsr){
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
    float r1 = opsl[i] * opsr[i] + t1;
    float r2 = opsl[i+1] * opsr[i+1] + t2;

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
      float r1 = opsl[i] * opsr[i] + values[i+1][j];
      float r2 = opsl[j] * opsr[j] + values[i][j-1];
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

float Reorder::getCost(float a, float b, Operation op){
  float c;
  if(op == Plus){
    return a + b;
  }
  if(a * b >= 0){
    c = a * b;
  } else{
    if(roundMode == FE_TONEAREST || roundMode == FE_TOWARDZERO){
      c = fabsf(a)*fabsf(b);
    }else if(roundMode == FE_DOWNWARD){
      fesetround(FE_UPWARD);
      c = fabsf(a) * fabs(b);
      fesetround(FE_DOWNWARD);
    }else{
      fesetround(FE_DOWNWARD);
      c = fabsf(a) * fabs(b);
      fesetround(FE_UPWARD);
    }
  }
  return c;
}



float Reorder::getValue(float a, float b, Operation op){
  if(op == Plus){
    return a + b;
  }else{
    return a * b;
  }
}
