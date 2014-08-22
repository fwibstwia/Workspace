#include <stdio.h>
#include <math.h>
double cal(double rx, double ry, double rz, double sx, double sy, double sz){
  double A = rx*rx + (rz*rz + ry*ry);
  double B = -2.0*(sx*rx + (sy*ry + sz*rz));
  double C = ((sx*sx + sy*sy) + sz*sz) - 14.128791;
  double D = B*B - 4 * A * C; 
  return D;
}
int main(){
  double rx = 0.125;
  double ry = -1.0450602;
  double rz = 3.2025185;
  double sx = -1.6350498;
  double sy = 4.8326674;
  double sz = -3.6311855;
  double r = cal(rx, ry, rz, sx, sy, sz);
  printf("%20.18f\n", r);
}
