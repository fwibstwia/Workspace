#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <glib.h>
#include <mpfr.h>

#define COUNT 3
#define MIN_RAND -10.0
#define MAX_RAND 10.0

struct mult {
  double left;
  double right;
};

struct orderInfo{
  double value;
  char* order;
};

struct mult nums[COUNT];
double inter_nums[COUNT];

int choice[COUNT][COUNT];
double cost[COUNT][COUNT];


void init(){
  srand((unsigned int) time(NULL));

  for(int i = 0; i < COUNT; i ++){
    nums[i].left = MIN_RAND + (double)rand()/(double)(RAND_MAX) * (MAX_RAND - MIN_RAND);
    nums[i].right = MIN_RAND + (double)rand()/(double)(RAND_MAX) * (MAX_RAND - MIN_RAND);
    inter_nums[i] = nums[i].left * nums[i].right;
  }
}
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
    double v;
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
  result = g_array_new (FALSE, TRUE, sizeof (gdouble));  

  if(i == j){
    double r = nums[i].left * nums[i].right;
    g_array_append_val (result, r);
    return result;
  }

  for(k = i; k < j; k ++){
    GArray* subParens1 = getAllOrderResults(i,k);
    GArray* subParens2 = getAllOrderResults(k + 1, j);
    double temp;
    if(subParens1 -> len == 1 || subParens2 -> len == 1){
      if(subParens1 -> len == 1){
	for(int m = 0; m < subParens2 -> len; m ++){
	  temp = nums[i].left * nums[i].right + g_array_index (subParens2, gdouble, m);
	  g_array_append_val(result, temp);
	}
      }

      if(subParens2 -> len == 1){
	for(int m = 0; m < subParens1 -> len; m ++){
	  temp = nums[j].left * nums[j].right + g_array_index (subParens1, gdouble, m);
	  g_array_append_val(result, temp);
	}
      }
    }else{
      for(int m = 0; m < subParens1 -> len; m ++ ){
	for(int n = 0; n < subParens2 -> len; n ++){
	  temp = g_array_index(subParens1, gdouble, m) + g_array_index(subParens2, gdouble, n);
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
  double* r1  = (double *) a;
  double* r2 = (double *) b;
  if(*r1 > *r2){
    return 1;
  }else if(*r1 < *r2){
    return -1;
  }
  return 0;
}

void getBound(int direction){
  for(int i = 0; i < COUNT - 1; i ++){
    //double t1 = nums[i+1].left * nums[i+1].right;
    //double t2 = nums[i].left * nums[i].right;
    double r1 = nums[i].left * nums[i].right + nums[i+1].left * nums[i+1].right;
    double r2 = nums[i+1].left * nums[i+1].right + nums[i].left * nums[i].right;
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
      double r1 = nums[i].left * nums[i].right + cost[i+1][j];
      double r2 = nums[j].left * nums[j].right + cost[i][j-1];
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
	double t = cost[i][k] + cost[k+1][j];
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
  double nonFmaR = nums[0].left * nums[0].right;
  double fmaR = nums[0].left * nums[0].right;
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

int main(int argc, char *argv[]){
  init();
  double rx = 0.125, ry = -1.0450602, rz = 3.2025185,
    sx = -1.6350498, sy = 4.8326674, sz = -3.6311855;
  GArray* allResults;
  GArray* allInterResults;
  nums[0].left = rx;
  nums[0].right = rx;
  nums[1].left = ry;
  nums[1].right = ry;
  nums[2].left = rz;
  nums[2].right = rz;
  allResults = getAllOrderResults(0, COUNT - 1);
  g_array_sort(allResults, float_sort);
  printf("All fma min: %f\n", g_array_index(allResults, gdouble, 0));
  printf("All fma max: %f\n", g_array_index(allResults, gdouble, (allResults -> len) - 1));
  getBound(0);
  double rMin = cost[0][COUNT-1];
  printf("%f\n", cost[0][COUNT-1]);
  //printf("%s\n", constructParens(0, COUNT - 1));
  getBound(1);
  double rMax = cost[0][COUNT-1];
  printf("%f\n", cost[0][COUNT-1]);
  //printf("%s\n", constructParens(0, COUNT - 1));
  g_array_free(allResults, TRUE);
  

  nums[0].left = rx;
  nums[0].right = sx;
  nums[1].left = ry;
  nums[1].right = sy;
  nums[2].left = rz;
  nums[2].right = sz;
  allResults = getAllOrderResults(0, COUNT - 1);
  g_array_sort(allResults, float_sort);
  printf("All fma min: %f\n", g_array_index(allResults, gdouble, 0));
  printf("All fma max: %f\n", g_array_index(allResults, gdouble, (allResults -> len) - 1));
  getBound(0);
  double rsMin = cost[0][COUNT-1];
  printf("%f\n", cost[0][COUNT-1]);
  //printf("%s\n", constructParens(0, COUNT - 1));
  getBound(1);
  double rsMax = cost[0][COUNT-1];
  printf("%f\n", cost[0][COUNT-1]);
  //printf("%s\n", constructParens(0, COUNT - 1));
  g_array_free(allResults, TRUE);
  /*
  inter_nums[0] = nums[0].left * nums[0].right;
  inter_nums[1] = nums[1].left * nums[1].right;
  inter_nums[2] = nums[2].left * nums[2].right;
  allInterResults = getAllInterOrderResults(0, COUNT - 1);
  g_array_sort(allInterResults, order_sort); 
  struct orderInfo nonFMAMin = g_array_index(allInterResults, struct orderInfo, 0);
  printf("All non-fma min: %f\n", nonFMAMin.value);
  printf("All non-fma min: %s\n", nonFMAMin.order);
  struct orderInfo nonFMAMax = g_array_index(allInterResults, struct orderInfo, (allInterResults -> len) - 1);
  printf("All non-fma max: %f\n", nonFMAMax.value);
  printf("All non-fma max: %s\n", nonFMAMax.order);
  */

  nums[0].left = sx;
  nums[0].right = sx;
  nums[1].left = sy;
  nums[1].right = sy;
  nums[2].left = sz;
  nums[2].right =sz;
  allResults = getAllOrderResults(0, COUNT - 1);
  g_array_sort(allResults, float_sort);
  printf("All fma min: %f\n", g_array_index(allResults, gdouble, 0));
  printf("All fma max: %f\n", g_array_index(allResults, gdouble, (allResults -> len) - 1));
  getBound(0);
  double sMin = cost[0][COUNT-1];
  printf("%f\n", cost[0][COUNT-1]);
  //printf("%s\n", constructParens(0, COUNT - 1));
  getBound(1);
  double sMax = cost[0][COUNT-1];
  printf("%f\n", cost[0][COUNT-1]);
  // printf("%s\n", constructParens(0, COUNT - 1));
  g_array_free(allResults, TRUE);
 
  
  double max = 4 * rsMin * rsMin - 4 * rMin * (sMin - 14.128791f);
  double min = 4 * rsMax * rsMax - 4 * rMax * (sMax - 14.128791f);
  printf("%f\n", max);
  printf("%f\n", min);
 
/*compareResults();
  getInfResult();
  for(int i = 0; i < COUNT; i ++){
    printf("%f\n", nums[i].left);
    printf("%f\n", nums[i].right);
  }
  allInterResults = getAllInterOrderResults(0, COUNT - 1);
  g_array_sort(allInterResults, order_sort); 
  struct orderInfo nonFMAMin = g_array_index(allInterResults, struct orderInfo, 0);
  printf("All non-fma min: %f\n", nonFMAMin.value);
  printf("All non-fma min: %s\n", nonFMAMin.order);
  struct orderInfo nonFMAMax = g_array_index(allInterResults, struct orderInfo, (allInterResults -> len) - 1);
  printf("All non-fma max: %f\n", nonFMAMax.value);
  printf("All non-fma max: %s\n", nonFMAMax.order);
  allResults = getAllOrderResults(0, COUNT - 1);
  g_array_sort(allResults, float_sort);
  printf("All fma min: %f\n", g_array_index(allResults, gdouble, 0));
  printf("All fma max: %f\n", g_array_index(allResults, gdouble, (allResults -> len) - 1));
  getBound(0);
  printf("%f\n", cost[0][COUNT-1]);
  printf("%s\n", constructParens(0, COUNT - 1));
  getBound(1);
  printf("%f\n", cost[0][COUNT-1]);
  printf("%s\n", constructParens(0, COUNT - 1));
  g_array_free(allResults, TRUE);
  g_array_free(allInterResults, TRUE);*/
}

