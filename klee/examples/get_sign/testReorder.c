#include <klee/klee.h>

int get_sign(float a, float b, float c) {
  float A = a + b + c;
  klee_tag_reorderable(&A);
  if (A > 10)
     return 0;
} 
int main() {
  float a;
  float b;
  float c;
  klee_make_symbolic_with_sort(&a, sizeof(a), "a", 0, 32);
  klee_make_symbolic_with_sort(&b, sizeof(b), "b", 0, 32);
  klee_make_symbolic_with_sort(&c, sizeof(c), "c", 0, 32);
  return get_sign(a, b, c);
} 
