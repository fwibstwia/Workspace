#include <klee/klee.h>

int ray_tracing(float rx, float ry, float rz, float sx, float sy, float sz) {
  /*float A = rx*rx+ry*ry+rz*rz;
  klee_tag_reorderable(&A, 0, 2);

  float B1 = sx*rx + sy*ry + sz*rz;
  klee_tag_reorderable(&B1, 0, 2); 
  float B = -2.0 * B1;
  */
  sx = 14.128791f;
  float C1 = sx * sy;
  //klee_tag_reorderable(&C1, 0, 2);
  if (C1 > 2)
    return 0;
} 

int main() {
  float rx, ry, rz, sx, sy, sz;
  klee_make_symbolic_with_sort(&rx, sizeof(rx), "rx", 0, 32);
  klee_make_symbolic_with_sort(&ry, sizeof(ry), "ry", 0, 32);
  klee_make_symbolic_with_sort(&rz, sizeof(rz), "rz", 0, 32);
  klee_make_symbolic_with_sort(&sx, sizeof(sx), "sx", 0, 32);
  klee_make_symbolic_with_sort(&sy, sizeof(sy), "sy", 0, 32);
  klee_make_symbolic_with_sort(&sz, sizeof(sz), "sz", 0, 32);
  return ray_tracing(rx, ry, rz, sx, sy, sz);
} 
