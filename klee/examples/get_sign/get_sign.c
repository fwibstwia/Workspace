/*
 * First KLEE tutorial: testing a small function
 */

#include <klee/klee.h>

int get_sign(int x) {
  int result[3];
  int i = 0;
  while (i < 3){
    result[i] = i;
    i ++;
  }
} 

int main() {
  int a;
  klee_make_symbolic_with_sort(&a, sizeof(a), "a", 0, 32);
  return get_sign(a);
} 
