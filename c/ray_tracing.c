/*
 * First KLEE tutorial: testing a small function
 */

#include <stdio.h>

int ray_tracing(float *r, float *s, float radius) {
  float A = 0; 
  int i;
  for(i = 0; i < 3; i ++){
    A = A + r[i]*r[i];
  }  
  
  float B1 = 0;
  for(i = 0; i < 3; i ++){
    B1 = B1 + s[i]*r[i];
  }

  float B = -2.0 * B1;
  
  float C1 = 0;
  for(i = 0; i < 3; i ++){
    C1 = C1 + s[i]*s[i];
  }

  float C =  C1 - radius;
  float D = B*B - 4*A*C;
  //float D = A + B - C ;*/ 
  printf("%f\n", D);  
           
  if (D > 0)
    return 0;
} 

int main() {
  float r[3], s[3];
  float radius;
  r[0] = -47.999900817871094f;
  r[1] = -54.000000000000000f;
  r[2] = -53.000000000000000f;
  s[0] = -47.111232757568359f;
  s[1] = -53.000000000000000f;
  s[2] = -52.018554687500000f;
  radius = 0.000000029802322f;
  return ray_tracing(r, s, radius);
} 

