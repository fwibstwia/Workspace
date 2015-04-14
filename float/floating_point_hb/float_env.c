#include <stdio.h>
#include <math.h>

#pragma STDC FP_CONTRACT OFF

int main(){
  double a, b;
  a = b = 1.0;
  while(a + 1.0 - a == 1.0)
    a = a * 2;
  while((a + b) - a != b)
    b += 1.0;
  printf( "Radix b = %g\n", b);
  return 0;
}
