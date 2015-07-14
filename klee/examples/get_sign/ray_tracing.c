#include <klee/klee.h>
#include <stdlib.h>

int raySphere(float *r, float *s, float radius) {
  int i = 0;
  int f[3];
  while(i < 3){
    f[i] = i;
    i ++;
  }
 
  return 0;
} 

int main() {
  float r[3], s[3];
  float radius;
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 32);
  klee_make_symbolic_with_sort(&s, sizeof(s), "s", 8, 32);
  klee_make_symbolic_with_sort(&radius, sizeof(radius), "radius", 0, 32);
  return raySphere(r, s, radius);
} 
