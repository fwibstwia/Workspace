/*
 * First KLEE tutorial: testing a small function
 */

#include <klee/klee.h>

int get_sign(float x, float y) {
  x = x + 2.0;
  x = x * 2.0;
  y = x * 3;
  if (x + y > 0)
     return 0;
} 

int main() {
  float a, b;
  klee_make_symbolic_with_sort(&a, sizeof(a), "a", 0, 32);
  klee_make_symbolic_with_sort(&b, sizeof(b), "a", 0, 32);
  return get_sign(a, b);
} 
