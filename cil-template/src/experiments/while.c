#include <stdio.h>


int main()
{
  float a0, a1, a2, b0, b1, b2, radius;

  float A = a0 * a0 + a1 * a1 + a2*a2;
  float B = 2.0*(a0 * b0 + a1 * b1 + a2 * b2);
  float C = b0*b0 + b1*b1 + b2*b2 + radius;    
  float D = B*B + A*C; 

  return 0;
}
