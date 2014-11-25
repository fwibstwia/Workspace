#include <klee/klee.h>

int ray_tracing(float *r, float radius) {
  float A = 0;  
  int i;
 r[0] = 0.1171875000f;
 r[1] = 0.4482421875f;
 r[2]= -0.1645507812f;
 r[3] = -0.2609863281f;
 r[4] = 0.1048583984f;
 r[5] = 0.087890625f;
 r[6] = -0.25f;
 r[7] = -0.125f;
 r[8] = 1.28125f;
  for(i = 0; i < 9; i ++){
      A = A + r[i]*r[i];
  }       
  klee_tag_reorderable(&A, 1, 2);      
  if (A + radius > 5)
    return 0;
} 

int main() {
  float r[9];
  float radius;
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 32);
  klee_make_symbolic_with_sort(&radius, sizeof(radius), "r", 8, 32);
  return ray_tracing(r, radius);
} 
