#include <klee/klee.h>

int ray_tracing(float *r, float rz, float sx, float sy, float sz) {
  float A = r[0]*r[0]+r[1]*r[1]+rz*rz;
  float B = -2.0 * (sx*r[0]+sy*r[1]+sz*rz);
  float C = sx*sx+sy*sy+sz*sz - 2;
  float D = B*B-4*A*C;                  
  if (D > 0)
    return 0;
} 

int main() {
  float r[2];
  float rz, sx, sy, sz;
  klee_make_symbolic_with_sort(&r, sizeof(r), "a", 8, 32);
  klee_make_symbolic_with_sort(&rz, sizeof(rz), "a", 0, 32);
  klee_make_symbolic_with_sort(&sx, sizeof(sx), "a", 0, 32);
  klee_make_symbolic_with_sort(&sy, sizeof(sy), "a", 0, 32);
  klee_make_symbolic_with_sort(&sz, sizeof(sz), "a", 0, 32);
  return ray_tracing(r, rz, sx, sy, sz);
} 
