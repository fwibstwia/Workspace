#include <klee/klee.h>

float f_orientation(float* p, float* q, float* r) {
    float det = (q[0]-p[0])*(r[1]-p[1]) + (p[1]-q[1])*(r[0]-p[0]);
    klee_tag_reorderable(&det, 1, 2);  
    return det;
}

float f_extended(float* p, float* q, float* r) {
     float l = (q[0]-p[0]) * (r[0]-q[0]) + (q[1] - p[1]) * (r[1]-q[1]);
     klee_tag_reorderable(&l, 1, 2);
     return l;
}

int extended_rightturn(float* p, float* q, float* r) {
    float det = f_orientation(p,q,r);
    if(det < 0.0 || (det == 0.0 && f_extended(p,q,r) >= 0.0)){
	return 1;
    }
    return 0;
}

// extended_rightturn: Returns true if pqr form a rightturn, or if pqr are 
// collinear and if r is on the extension of the ray starting in q in
// the direction q-p, i.e., if (q-p)*(r-q) >= 0.
// All points must be from the range [first,last), which will be used
// for computing indices in the verbose trace output.
int area_sign(float* a, float* b, float* c, float *d) {
    float ax, ay, az, bx, by, bz, cx, cy, cz;
    float vol;
    ax = a[0] - d[0];
    ay = a[1] - d[1];
    az = a[2] - d[2];
    bx = b[0] - d[0];
    by = b[1] - d[1];
    bz = b[2] - d[2];
    cx = c[0] - d[0];
    cy = c[1] - d[1];
    cz = c[2] - d[2];

    float yz = by*cz + (-bz)*cy;
    klee_tag_reorderable(&yz, 0, 2);
    float zx = bz*cx + (-bx)*cz;
    klee_tag_reorderable(&zx, 0, 2);
    float xy = bx*cy + (-by)*cx;
    klee_tag_reorderable(&xy, 0, 2);

    vol = ax*yz + ay*zx + az*xy;
    klee_tag_reorderable(&vol, 0, 2);

    if(vol > 0.5)
       return 1;
}

int main() {
/*
  float p[2], q[2], r[2];
  klee_make_symbolic_with_sort(&p, sizeof(p), "p", 8, 32);
  klee_make_symbolic_with_sort(&q, sizeof(q), "q", 8, 32);
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 32);
  return extended_rightturn(p, q, r);*/

  float a[3], b[3], c[3], d[3];
  klee_make_symbolic_with_sort(&a, sizeof(a), "a", 8, 32);
  klee_make_symbolic_with_sort(&b, sizeof(b), "b", 8, 32);
  klee_make_symbolic_with_sort(&c, sizeof(c), "c", 8, 32);
  klee_make_symbolic_with_sort(&d, sizeof(d), "d", 8, 32);
  return area_sign(a, b, c, d);
}
