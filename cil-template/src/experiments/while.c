#include <stdio.h>


int main()
{
  float A, B, C, D, E, F, G;
  float H = A * A + B * B + C*C;
  float I = -2.0*(A*D + B*E + C*F);
  float J = D*D + E*E + F*F - G;
  float K = -4.0*H*J;
  /*
  float a0, a1, a2, b0, b1, b2, radius;
  int i = 0;
  float A = a0 * a0 + a1 * a1 + a2*a2;
  float B = -2.0*(a0 * b0 + a1 * b1 + a2 * b2);
  float C = b0*b0 + b1*b1 + b2*b2 - radius;    
  float D = B*B - 4.0*A*C;
  if(D < 0.0){
    A = B - C;
  }*/

  return 0;
}
/* 
[1,2] ... [1,2]
D \in [-456.00006103515625, 528.00006103515625]
[1,1] ... [1,1]
D \in [11.9999942779541015625, 12.0000057220458984375]
*/

