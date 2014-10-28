/*
 * First KLEE tutorial: testing a small function
 */

#include <klee/klee.h>
int f(int x){
  return x + g(x);
}

int g(int x) { return 7; }

int get_sign(int x) {
  //x = x + 5; 
  int z = 0;
  for (z = 0; z < 3; ++z) {
    x = x + z; //f(x);
  }
  x = x * x;
  if (x == 0)
     return 0;
} 

int main() {
  int a;
  klee_make_symbolic_with_sort(&a, sizeof(a), "a", 0, 32);
  return get_sign(a);
} 
