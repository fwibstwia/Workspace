#include <stdio.h>
#include <immintrin.h>

#pragma STDC FP_CONTRACT ON
float testFMAOrder(float a, float b, float c, float d){
  float r= a- b*c*d;
  return r;
}

int main(){
  float r, a, b, c, d;
  r = testFMAOrder(a, b, c, d);
}
