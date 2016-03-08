#include <stdio.h>


int main()
{
  /*
  float A, B;
  float C = A*A;
  float D = A*B;
  float E = B*B;
  float F = D*D - C*E;*/
  
  
  float A, B, C, D, E, F, G;
  float H = A * A + B * B + C*C;
  float I = -2.0*(A*D + B*E + C*F);
  float J = D*D + E*E + F*F - G;
  float K = I*I-4.0*H*J;
  if(K < 0.0){
    return 1;
  }
  

  return 0;
}
/* 
[1,2] ... [1,2]
D \in [-456.00006103515625, 528.00006103515625]
[1,1] ... [1,1]
D \in [11.9999942779541015625, 12.0000057220458984375]
*/

