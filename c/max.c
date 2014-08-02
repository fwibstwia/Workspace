#include <stdio.h>
#include <math.h>
int main(){
  float rx = 0.125;
  float ry = -1.0450602;
  float rz = 3.2025185;
  float sx = -1.6350498;
  float sy = 4.8326674;
  float sz = -3.6311855;
  float A = rx*rx + ry*ry + rz*rz;
  float B = -2.0*(sx*rx + (sy*ry + sz*rz));
  float C = ((sx*sx + sy*sy) + sz*sz) - 14.128790;
  float D = B*B - 4 * A * C;
  printf("%f\n",sz);
  printf("%f\n", (sx*rx + (sy*ry + sz*rz)));
  printf("%f\n",D);
}
