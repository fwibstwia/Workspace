#include <stdio.h>

int main(){
  float rx = 0.125;
  float ry = -1.0450602;
  float rz = 3.2025185;
  float sx = -1.635050;
  float sy = 4.832667;
  float sz = -3.6311855;
  float A = rx*rx + (ry*ry + rz*rz);
  float B = -2.0*((sx*rx + sy*ry) + sz*rz);
  float C = (sx*sx + (sy*sy + sz*sz)) - 14.128790;
  float D = B*B - 4 * A * C;
  printf("%f\n", A);
  printf("%f\n", (sx*rx + sy*ry) + sz*rz);
  printf("%f\n", sx*sx + (sy*sy + sz*sz));
  printf("%f\n",D);
}
