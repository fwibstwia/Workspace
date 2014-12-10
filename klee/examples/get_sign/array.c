#include <klee/klee.h>

int ray_tracing(float *r, float radius) {
  float A = 0;  
  int i;
  for(i = 0; i < 6; i ++){
      A = A + r[i];
  }       
  klee_tag_reorderable(&A, 1, 0);      
  if (A > 100)
    return 0;
} 

int main() {
  float r[6];
  float radius;
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 32);
  klee_make_symbolic_with_sort(&radius, sizeof(radius), "r", 8, 32);
  return ray_tracing(r, radius);
} 
