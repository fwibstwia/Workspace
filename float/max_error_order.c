#include <stdio.h>
#include <stdlib.h>
#include <mpfr.h>

#define MAX_SCOPE 1000
#define MAX_SUMS 1000

float generateFloatNumber();
float getAllSums(float* f, float fs[], int size, char o[]);

float all_sums[MAX_SUMS];
char* sum_orders[MAX_SUMS];
int index;

int main() {
  mpfr_t s,t;
  int num = 5;
  int i = 0;
  float max_sum = 0.0;
  float* nums = (float*)malloc(sizeof(float) * num);
  
  mpfr_init2(s, 256);
  mpfr_init2(t, 256);
  mpfr_set_flt(s, 0, MPFR_RNDN);

  for(; i < num; i++){
    nums[i] = generateFloatNumber();
    printf("%f\n", nums[i]);
    mpfr_set_flt(t, nums[i], MPFR_RNDN);
    mpfr_add (s, s, t, MPFR_RNDN);
  }
  printf ("The high presion sum is ");
  mpfr_out_str (stdout, 10, 0, s, MPFR_RNDN);
  mpfr_clear(s);
  mpfr_clear(t);
  free(nums);
  return 0;
}

float getAllSums(float nums[], int size, int signal){
  float* input_1, input_2; 
  float sum;
  int i;
  if(size > 2){
    input_1 = (float*)malloc(sizeof(float) * (size - 1));
    input_2 = (float*)malloc(sizeof(float) * (size - 1));
    
    for(i = 0; i < size - 1; i++){
      if(i == 0){
	input_1[i] = nums[0] + nums[1];
      }else{
	input_1[i] = nums[i + 1];
      }
    }

    getAllSums(f, input_1, size - 1, o, signal);

    for(i = 0; i < size - 1; i++){
      input_2[i] = nums[i + 1]; 
    }
    
    sum = nums[0] + getAllSums(f, input_2, size - 1, o, 0);
    
    if(signal == 1){  
      if(sum > *f){
	*f = sum
      }
    }

    free(input_1);
    free(input_2);
    return sum;
  }else{
    if(signal == 0){
      
    }else{

    }
  }
}

float generateFloatNumber() {
  return ((float)rand()/RAND_MAX)*MAX_SCOPE;
}
