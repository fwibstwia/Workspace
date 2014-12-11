/*
 * First KLEE tutorial: testing a small function
 */

#include <klee/klee.h>

int ray_tracing(float *r, float *s, float radius) {
  float A = 0; 
  int i;
  for(i = 0; i < 3; i ++){
    A = A + r[i]*r[i];
  }  
  
  klee_tag_reorderable(&A, 1, 2);
  float B1 = 0;
  for(i = 0; i < 3; i ++){
    B1 = B1 + s[i]*r[i];
  }

  klee_tag_reorderable(&B1, 1, 2); 
  float B = -2.0 * B1;
  
  float C1 = 0;
  for(i = 0; i < 3; i ++){
    C1 = C1 + s[i]*s[i];
  }

  klee_tag_reorderable(&C1, 1, 2);
  float C =  C1 - radius;
  float D = B*B - 4*A*C;
  //float D = A + B - C ;*/              
  if (D > 0)
    return 0;
} 

int main() {
  float r[3], s[3];
  float radius;
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 32);
  klee_make_symbolic_with_sort(&s, sizeof(s), "s", 8, 32);
  klee_make_symbolic_with_sort(&radius, sizeof(radius), "radius", 0, 32);
  return ray_tracing(r, s, radius);
} 

