/*
 * First KLEE tutorial: testing a small function
 */

#include <klee/klee.h>

int ray_tracing(float rx, float ry, float rz, float sx, float sy, float sz) {
  float A = rx*rx+ry*ry+rz*rz;
  float B = -2.0 * (sx*rx+sy*ry+sz*rz);
  float C = sx*sx+sy*sy+sz*sz - 2;
  float D = B*B-4*A*C;                  
  if (D > 0)
    return 0;
} 

int main() {
  float rx, ry, rz, sx, sy, sz;
  klee_make_symbolic_with_sort(&rx, sizeof(rx), "a", 0, 32);
  klee_make_symbolic_with_sort(&ry, sizeof(ry), "a", 0, 32);
  klee_make_symbolic_with_sort(&rz, sizeof(rz), "a", 0, 32);
  klee_make_symbolic_with_sort(&sx, sizeof(sx), "a", 0, 32);
  klee_make_symbolic_with_sort(&sy, sizeof(sy), "a", 0, 32);
  klee_make_symbolic_with_sort(&sz, sizeof(sz), "a", 0, 32);
  return ray_tracing(rx, ry, rz, sx, sy, sz);
} 

