#include <stdio.h>
#include <immintrin.h>

void testFMAOrder(float a, float b, float c){
  __m128 e, f, g;
  e[0] = a;
  f[0] = b;
  g[0] = c;
  __m128 r = _mm_fmadd_ps(e, f, g);
  printf("%f", r[0]);
  return; 
}

int main(){
  testFMAOrder(1, 2, 3);
}
