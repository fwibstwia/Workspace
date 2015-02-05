#include <klee/klee.h>
float f_orientation(float* p, float* q, float* r) {
    float det = (q[0]-p[0])*(r[1]-p[1]) + (p[1] - q[1])*(r[0]-p[0]);
    klee_tag_reorderable(&det, 1, 2);  
    return det;
}

float f_extended(float* p, float* q, float* r) {
     float l = (q[0]-p[0]) * (r[0]-q[0]) + (q[1] - p[1]) * (r[1]-q[1]);
     //klee_tag_reorderable(&l, 1, 2);
     return l;
}

// extended_rightturn: Returns true if pqr form a rightturn, or if pqr are 
// collinear and if r is on the extension of the ray starting in q in
// the direction q-p, i.e., if (q-p)*(r-q) >= 0.
// All points must be from the range [first,last), which will be used
// for computing indices in the verbose trace output.
int extended_rightturn(float* p, float* q, float* r) {
    float det = f_orientation(p,q,r);
    float extended = f_extended(p,q,r);
    if (extended >= 1){
    	if(det > 0.1f){
	   return 1;
    	}
    }
    return 0;
}

int main() {
  float p[2], q[2], r[2];
  klee_make_symbolic_with_sort(&p, sizeof(p), "p", 8, 32);
  klee_make_symbolic_with_sort(&q, sizeof(q), "q", 8, 32);
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 32);
  return extended_rightturn(p, q, r);
}
