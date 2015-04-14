#include <klee/klee.h>
struct Point {
    float x, y;
};
typedef struct Point Point;

float f_orientation(Point* p, Point* q, Point* r) {
    float det = (q->x-p->x)*(r->y-p->y) + (q->y-p->y)*(r->x-p->x);
    klee_tag_reorderable(&det, 1, 2);  
    return det;
}

float f_extended(Point* p, Point* q, Point* r) {
     float l = (q->x-p->x) * (r->x-q->x) + (q->y - p->y) * (r->y-q->y);
     klee_tag_reorderable(&l, 1, 2);
     return l;
}

// extended_rightturn: Returns true if pqr form a rightturn, or if pqr are 
// collinear and if r is on the extension of the ray starting in q in
// the direction q-p, i.e., if (q-p)*(r-q) >= 0.
// All points must be from the range [first,last), which will be used
// for computing indices in the verbose trace output.
int extended_rightturn(Point* p, Point* q, Point* r) {
    float det = f_orientation(p,q,r);
    if(det < 0.0 || (det == 0.0 && f_extended(p,q,r) >= 0.0)){
	return 1;
    }
    return 0;
}

int main() {
  Point p, q, r;
  klee_make_symbolic_with_sort(&p, sizeof(p), "p", 8, 64);
  klee_make_symbolic_with_sort(&q, sizeof(q), "q", 8, 64);
  klee_make_symbolic_with_sort(&r, sizeof(r), "r", 8, 64);
  return extended_rightturn(&p, &q, &r);
}
