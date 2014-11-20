#include <klee/klee.h>

int ray_tracing(float *r) {
  float A = 0;  
  int i = 0;
  r[0] = 17.988571f;
  r[1] = 19.597578f;
  r[2] = 135.595596f;
  for(i = 0; i < 30; i ++){
      A = A + r[i];
  }             
  if (A > 5)
    return 0;
} 

int main() {
  float r[30];
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 32);
  return ray_tracing(r);
} 
