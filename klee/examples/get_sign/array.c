#include <klee/klee.h>

int ray_tracing(float *r) {
  float A = r[0]*r[0]+r[1]*r[1]+r[2]*r[2];               
  if (A > 0)
    return 0;
} 

int main() {
  float r[3];
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 32);
  return ray_tracing(r);
} 
