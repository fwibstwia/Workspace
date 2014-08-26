#include <stdio.h>
#include <stdlib.h>
#include <fenv.h>
#include <math.h>
#include <glib.h>
#include <mpfr.h>

#include "reorder.h"

#define COUNT 500

struct orderInfo{
  float value;
  char* order;
};

int choice[COUNT][COUNT];
float value[COUNT][COUNT];
float nums[COUNT];

int currRound;

GArray* getAllOrders(int i, int j){
  GArray* result;
  struct orderInfo o;
  result = g_array_new (FALSE, TRUE, sizeof (struct orderInfo));
  if(i == j){
    char buf[1024];
    snprintf (buf, sizeof(buf), "%f", nums[i]);
    o.order = strdup(buf);
    o.value = nums[i];
    g_array_append_val(result, o);
  }

  for(int k = i; k < j; k ++){
    GArray* subParens1 = getAllOrders(i, k);
    GArray* subParens2 = getAllOrders(k + 1, j);
    for(int m = 0; m < subParens1 -> len; m ++){
      struct orderInfo s1 = g_array_index(subParens1, struct orderInfo, m);
      for(int n = 0; n < subParens2 -> len; n ++){
	char buf[1024];
	struct orderInfo s2 = g_array_index(subParens2, struct orderInfo, n);
	if(op == 1){
	  o.value = s1.value * s2.value;
	}else{
	  o.value = s1.value + s2.value;
	}
	strcpy (buf, "(");
	strcat (buf, s1.order);
	strcat (buf, "+");
	strcat (buf, s2.order);
	strcat (buf, ")");
        o.order = strdup(buf);
	g_array_append_val(result, o);
      }
    }

    for(int m = 0; m < subParens1 -> len; m ++){
      struct orderInfo s1 = g_array_index(subParens1, struct orderInfo, m);
      free(s1.order);
    }
    
    for(int n = 0; n < subParens2 -> len; n ++){
      struct orderInfo s2 = g_array_index(subParens2, struct orderInfo, n);
      free(s2.order);
    }

    g_array_free(subParens1, TRUE);
    g_array_free(subParens2, TRUE);
  }
  return result;
}

float cost(float a, float b){
  
  float c;
  if(a * b >= 0){
    c = a*b;
  } else{
    if(currRound == FE_TONEAREST || currRound == FE_TOWARDZERO){
      c = fabsf(a)*fabsf(b);
    }else if(currRound == FE_DOWNWARD){
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

void getBound(int direction, int op){
  float c, t;
  for(int i = 0; i < COUNT; i ++){
    value[i][i] = nums[i];
  }
  
  for(int l = 2; l <= COUNT; l ++){
    for(int i = 0; i < COUNT - l + 1; i ++){
      int j = i + l - 1;
      choice[i][j] = i;
      if(op == 1){
	c = cost(value[i][i], value[i+1][j]);
	value[i][j] = value[i][i] * value[i+1][j];	
      }else{
	c = value[i][i] + value[i+1][j];
	value[i][j] = value[i][i] + value[i+1][j];
      }
      for(int k = i + 1;  k < j; k++){
	if(op == 1){
	  t = cost(value[i][k], value[k+1][j]);
	}else{
	  t = value[i][k] + value[k+1][j];
	}

	if(direction == 1){
	  if(t > c){
	    c = t;
	    choice[i][j] = k;
	    if(op == 1){
	      value[i][j] = value[i][k] * value[k+1][j];
	    }else{
	      value[i][j] = value[i][k] + value[k+1][j];
	    }
	  }
	}else{
	  if(t < c){
	    c = t;
	    choice[i][j] = k;
	    if(op == 1){
	      value[i][j] = value[i][k] * value[k+1][j];
	    }else{
	      value[i][j] = value[i][k] + value[k+1][j];
	    }
	  }
	}
      }     
    }    
  }
}

int order_sort(gconstpointer a, gconstpointer b){
  struct orderInfo* r1 = (struct orderInfo*) a;
  struct orderInfo* r2 = (struct orderInfo*) b;
  if((*r1).value > (*r2).value){
    return 1;
  }else if((*r1).value < (*r2).value){
    return -1;
  }
  return 0;
}

float getMax(int round_mode, int op, float* fs, int size){
 fesetround(round_mode);
 for(int i = 0; i < size; i++){
   nums[i] = fs[i];
 }
 getBound(0, op);
 return value[0][COUNT-1]; 
}

float getMin(int round_mode, int op, float* nums, int size){
 fesetround(round_mode);
 for(int i = 0; i < size; i++){
   nums[i] = fs[i];
 }
 getBound(1, op);
 return value[0][COUNT-1]; 
}


