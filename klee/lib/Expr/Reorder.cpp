#include "klee/Reorder.h"

using namespace std;

template<typename T>
T Reorder<T>::getBound(int direction, Operation op, const vector<T> &ops){
  T c, t;
  std::vector<std::vector<T> > values;
  std::vector<std::vector<int> > choices;
  int len = ops.size();
  int origRound = fegetround();

  values.resize(len * len);
  choices.resize(len * len);

  fesetround(roundMode);
  for(int i = 0; i < len; i ++){
    value[i][i] = ops[i];
  }
  
  for(int l = 2; l <= len; l ++){
    for(int i = 0; i < len - l + 1; i ++){
      int j = i + l - 1;
      choice[i][j] = i;
      c = getCost(value[i][i], value[i+1][j], op);
      value[i][j] = getValue(value[i][i], value[i+1][j], op);	
      for(int k = i + 1;  k < j; k++){
	t = getCost(value[i][k], value[k+1][j], op);
	if(direction == 1){
	  if(t > c){
	    c = t;
	    choice[i][j] = k;
	    value[i][j] = getValue(value[i][k], value[k+1][j], op);
	  }
	}else{
	  if(t < c){
	    c = t;
	    choice[i][j] = k;	    
	    value[i][j] = getValue(value[i][k], value[k+1][j], op);
	  }
	}
      }     
    }    
  }

  fesetround(origRound);
  return value[0][len - 1];
}

template<typename T>
T Reorder<T>::getCost(T a, T b, Operation op){
  T c;
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

template<typename T>
T Reorder<T>::getValue(T a, T b, Operation op){
  if(op == Plus){
    return a + b;
  }else{
    return a * b;
  }
}
