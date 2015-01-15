#include <klee/klee.h>
int get_sign(float *r){
float A = r[1] + r[1];
if(A > 0)
 return 0;
}
int main() {
  float r[2];
  klee_make_symbolic_with_sort(&r, sizeof(r), "a", 8, 32);
  return get_sign(r);
}
