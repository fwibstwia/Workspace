/*
 * First KLEE tutorial: testing a small function
 */

#include <klee/klee.h>

int ray_tracing(float *r, float *s) {
  s[0] = 17.988571f;
  s[1] = 19.597578f;
  s[2] = 135.595596f;
  //sx = 0.005782f ;
  //sy = 31.999878f;
  //sz = 0.111744f;
  float A = 0; 
  int i = 0;
  for(; i< 100; i ++){
    A = A + r[i]*r[i];
  }  
  
  klee_tag_reorderable(&A, 0, 2);

  float B1 = s[0]*r[0] + s[1]*r[1] + s[2]*r[2];
  klee_tag_reorderable(&B1, 0, 2); 
  float B = -2.0 * B1;

  float C1 = s[0]*s[0]+s[1]*s[1]+s[2]*s[2];
  klee_tag_reorderable(&C1, 0, 2);
  float C =  C1 - 14.128791f;
  float D = B*B-4*A*C;  
  //float D = A + B - C ;              
  if (D > 0)
    return 0;
} 

int main() {
  float r[100], s[3];
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 32);
  klee_make_symbolic_with_sort(&s, sizeof(s), "s", 8, 32);
  
  return ray_tracing(r, s);
} 

