#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <glib.h>
#include <mpfr.h>
#include "reorder.h"

#define COUNT 500
#define MIN_RAND -10.0
#define MAX_RAND 10.0

struct mult {
  float left;
  float right;
};

struct orderInfo{
  float value;
  char* order;
};

struct mult nums[COUNT];
float inter_nums[COUNT];

int choice[COUNT][COUNT];
float cost[COUNT][COUNT];



/*
GArray* getAllInterOrderResults(int i, int j){
  int k;
  GArray* result;
  struct orderInfo o;
  result = g_array_new (FALSE, TRUE, sizeof (struct orderInfo));
  if(i == j){
    char buf[1024];
    snprintf (buf, sizeof(buf), "%f", inter_nums[i]);
    o.order = strdup(buf);
    o.value = inter_nums[i];
    g_array_append_val(result, o);
  }

  for(int k = i; k < j; k ++){
    GArray* subParens1 = getAllInterOrderResults(i, k);
    GArray* subParens2 = getAllInterOrderResults(k + 1, j);
    float v;
    for(int m = 0; m < subParens1 -> len; m ++){
      struct orderInfo s1 = g_array_index(subParens1, struct orderInfo, m);
      for(int n = 0; n < subParens2 -> len; n ++){
	char buf[1024];
	struct orderInfo s2 = g_array_index(subParens2, struct orderInfo, n);
	o.value = s1.value + s2.value;
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
  }*/

GArray* getAllOrderResults(int i, int j){
  int k;  
  GArray* result;
  result = g_array_new (FALSE, TRUE, sizeof (gfloat));  

  if(i == j){
    float r = nums[i].left * nums[i].right;
    g_array_append_val (result, r);
    return result;
  }

  for(k = i; k < j; k ++){
    GArray* subParens1 = getAllOrderResults(i,k);
    GArray* subParens2 = getAllOrderResults(k + 1, j);
    float temp;
    if(subParens1 -> len == 1 || subParens2 -> len == 1){
      if(subParens1 -> len == 1){
	for(int m = 0; m < subParens2 -> len; m ++){
	  temp = nums[i].left * nums[i].right + g_array_index (subParens2, gfloat, m);
	  g_array_append_val(result, temp);
	}
      }

      if(subParens2 -> len == 1){
	for(int m = 0; m < subParens1 -> len; m ++){
	  temp = nums[j].left * nums[j].right + g_array_index (subParens1, gfloat, m);
	  g_array_append_val(result, temp);
	}
      }
    }else{
      for(int m = 0; m < subParens1 -> len; m ++ ){
	for(int n = 0; n < subParens2 -> len; n ++){
	  temp = g_array_index(subParens1, gfloat, m) + g_array_index(subParens2, gfloat, n);
	  g_array_append_val(result, temp);
	}
      }
    }
    g_array_free(subParens1, TRUE);
    g_array_free(subParens2, TRUE);
  }
  
  return result;
}

/*
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
*/

int float_sort(gconstpointer a, gconstpointer b){
  float* r1  = (float *) a;
  float* r2 = (float *) b;
  if(*r1 > *r2){
    return 1;
  }else if(*r1 < *r2){
    return -1;
  }
  return 0;
}

void getBound(int direction){
  for(int i = 0; i < COUNT - 1; i ++){
    float t1 = nums[i+1].left * nums[i+1].right;
    float t2 = nums[i].left * nums[i].right;
    float r1 = nums[i].left * nums[i].right + t1;
    float r2 = nums[i+1].left * nums[i+1].right + t2;
    if(direction == 1){
      if(r1 > r2){
	cost[i][i+1] = r1;
	choice[i][i+1] = i;
      }else{
	cost[i][i+1] = r2;
	choice[i][i+1] = i + 1;
      }
    }else{
      if(r1 < r2){
	cost[i][i+1] = r1;
	choice[i][i+1] = i;
      }else{
	cost[i][i+1] = r2;
	choice[i][i+1] = i + 1;
      }
    }
  }
  
  for(int l = 3; l <= COUNT; l ++){
    for(int i = 0; i < COUNT - l + 1; i ++){
      int j = i + l - 1;
      float r1 = nums[i].left * nums[i].right + cost[i+1][j];
      float r2 = nums[j].left * nums[j].right + cost[i][j-1];
      if(direction == 1){
	if(r1 > r2){
	  cost[i][j] = r1;
	  choice[i][j] = i;
	}else{
	  cost[i][j] = r2;
	  choice[i][j] = j - 1;
	}
      }else{
	if(r1 < r2){
	  cost[i][j] = r1;
	  choice[i][j] = i;
	}else{
	  cost[i][j] = r2;
	  choice[i][j] = j - 1;
	}
      }

      for(int k = i + 1; k < j - 1; k ++){
	float t = cost[i][k] + cost[k+1][j];
	if(direction == 1){
	  if(t > cost[i][j]){
	    cost[i][j] = t;
	    choice[i][j] = k;
	  }
	}else{
	  if(t < cost[i][j]){
	    cost[i][j] = t;
	    choice[i][j] = k;
	  }
	}
      }
    }    
  }
}

float getMaxWithFMA(int round_mode, float* fs1, float* fs2, int size){
  fesetround(round_mode);
  for(int i = 0; i < size; i ++){
    nums[i].left = fs1[i];
    nums[i].right = fs2[i];
  }
  getBound(0);
  return cost[0][COUNT-1];
}

float getMinWithFMA(int round_mode, float* fs1, float* fs2, int size){
  fesetround(round_mode);
  for(int i = 0; i < size; i ++){
    nums[i].left = fs1[i];
    nums[i].right = fs2[i];
  }
  getBound(1);
  return cost[0][COUNT-1];
}

/*
char* constructParens(int i, int j){
  char bufl[1024];
  char bufr[1024];
  char s[1024];
  if(i == j){
    snprintf (bufl, sizeof(bufl), "%f", nums[i].left);
    strcpy(s, bufl);
    strcat(s, "*"); 
    snprintf (bufr, sizeof(bufr), "%f", nums[i].right);
    strcat(s, bufr);
    return strdup(s);
  }
  
  if(i + 1 == j){
    if(choice[i][j] == i){
      strcpy(s, "((");
      snprintf (bufl, sizeof(bufl), "%f", nums[i].left);
      strcat(s, bufl);
      strcat(s, "*"); 
      snprintf (bufr, sizeof(bufr), "%f", nums[i].right);
      strcat(s, bufr);
      strcat(s, ")+");
      snprintf (bufl, sizeof(bufl), "%f", nums[i+1].left);
      strcat(s, bufl);
      strcat(s, "*"); 
      snprintf (bufr, sizeof(bufr), "%f", nums[i+1].right);
      strcat(s, bufr);
      strcat(s, ")");
      return strdup(s);
    }else{
      strcpy(s, "(");
      snprintf (bufl, sizeof(bufl), "%f", nums[i].left);
      strcat(s, bufl);
      strcat(s, "*"); 
      snprintf (bufr, sizeof(bufr), "%f", nums[i].right);
      strcat(s, bufr);
      strcat(s, "+(");
      snprintf (bufl, sizeof(bufl), "%f", nums[i+1].left);
      strcat(s, bufl);
      strcat(s, "*"); 
      snprintf (bufr, sizeof(bufr), "%f", nums[i+1].right);
      strcat(s, bufr);
      strcat(s, "))");
      return strdup(s);
    }
  }

  strcpy(s,"(");
  char* l = constructParens(i, choice[i][j]);
  char* r = constructParens(choice[i][j] + 1, j);
  strcat(s, l);
  strcat(s, "+");
  strcat(s, r);
  strcat(s, ")");
  free(l);
  free(r);
  return strdup(s);
}

void getInfResult(){
  int precision = 1024;
  mpfr_t r, op1, op2, temp;
  mpfr_init2(r, precision);
  mpfr_init2(op1, precision);
  mpfr_init2(op2, precision);
  mpfr_init2(temp, precision);
  mpfr_set_flt(r, 0.0, MPFR_RNDN);
  for(int i = 0; i < COUNT; i ++){
    mpfr_set_flt(op1, nums[i].left, MPFR_RNDN);
    mpfr_set_flt(op2, nums[i].right, MPFR_RNDN);
    mpfr_mul(temp, op1, op2, MPFR_RNDN);
    mpfr_add(r, r, temp, MPFR_RNDN);
  }
  mpfr_printf ("%.512Rf\n", r);
  }

void compareResults(){
  float nonFmaR = nums[0].left * nums[0].right;
  float fmaR = nums[0].left * nums[0].right;
  for(int i = 1; i < COUNT; i ++){
    nonFmaR = nonFmaR + nums[i].left * nums[i].right;
    fmaR = fmaf(nums[i].left, nums[i].right, fmaR);
  }
  if(nonFmaR > fmaR){
    printf("nonfma is greater than fma\n");
  }
  printf("non-fma result: %f\n", nonFmaR);
  printf("fma result: %f\n", fmaR);
  }*/


