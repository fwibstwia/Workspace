#include <klee/klee.h>

int fma_test(float a, float b, float c) {
  float C1 = a*b + c;
  if (C1 > 2)
    return 0;
  return 1;
} 

int main() {
  float a, b, c;
  klee_make_symbolic_with_sort(&a, sizeof(a), "a", 0, 32);
  klee_make_symbolic_with_sort(&b, sizeof(b), "b", 0, 32);
  klee_make_symbolic_with_sort(&c, sizeof(c), "c", 0, 32);
  return fma_test(a, b, c);
} 
