#include <stdio.h>
#include <stdlib.h>
#include <fenv.h>
#include <math.h>
#include <glib.h>
#include <mpfr.h>

#define COUNT 5
#define MIN_RAND -10.0
#define MAX_RAND 10.0

struct orderInfo{
  float value;
  char* order;
};

int choice[COUNT][COUNT];
float value[COUNT][COUNT];
float nums[COUNT];

void init(){
  srand((unsigned int) time(NULL));
  for(int i = 0; i < COUNT; i ++){
    nums[i] = MIN_RAND + (float)rand()/(float)(RAND_MAX) * (MAX_RAND - MIN_RAND);
  }
  nums[0] = 6.315;
  nums[1] = 3.14159;
  nums[2] = 1.6192;
  nums[3] = 1.2412;
  nums[4] = 3.2222;
}

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
	o.value = s1.value * s2.value;
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
  const int currRound = fegetround();
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

void getBound(int direction){
  for(int i = 0; i < COUNT; i ++){
    value[i][i] = nums[i];
  }
  
  for(int l = 2; l <= COUNT; l ++){
    for(int i = 0; i < COUNT - l + 1; i ++){
      int j = i + l - 1;
      float c = cost(value[i][i], value[i+1][j]);
      choice[i][j] = i;
      value[i][j] = value[i][i] * value[i+1][j];
      for(int k = i + 1;  k < j; k++){
	float t = cost(value[i][k], value[k+1][j]);

	if(direction == 1){
	  if(t > c){
	    c = t;
	    choice[i][j] = k;
	    value[i][j] = value[i][k] * value[k+1][j];
	  }
	}else{
	  if(t < c){
	    c = t;
	    choice[i][j] = k;
	    value[i][j] = value[i][k] * value[k+1][j];
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

int main(){
 GArray* allResults;
 fesetround(FE_UPWARD);
 init();
 allResults = getAllOrders(0, COUNT - 1);
 g_array_sort(allResults, order_sort);
 struct orderInfo min = g_array_index(allResults, struct orderInfo, 0);
 printf("All min: %f\n", min.value);
 struct orderInfo max = g_array_index(allResults, struct orderInfo, (allResults -> len) - 1);
 printf("All max: %f\n", max.value);
 getBound(0);
 float rsMin = value[0][COUNT-1];
 printf("%f\n", value[0][COUNT-1]);
 getBound(1);
 float rsMax = value[0][COUNT-1];
 printf("%f\n", value[0][COUNT-1]);
 g_array_free(allResults, TRUE);
}
