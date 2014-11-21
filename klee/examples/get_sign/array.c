#include <klee/klee.h>

int ray_tracing(float *r) {
  float A = 0;  
  int i;
  for(i = 0; i < 3; i ++){
      A = A + r[i]*r[i];
  }       
  klee_tag_reorderable(&A, 1, 2);      
  if (A > 5)
    return 0;
} 

int main() {
  float r[3];
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 32);
  return ray_tracing(r);
} 
