#include <math.h>
int main(){
  float x1, x2, y1, y2, x3, y3;
  float q1 = (x1*y1 + x2*y2) + x3*y3;
  float q2 = x1*y1 + (x2*y2 + x3*y3);
  assert(q1==q2);
}
