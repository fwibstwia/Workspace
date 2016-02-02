/*
 * First KLEE tutorial: testing a small function
 */

#include <stdio.h>

float dot3(float *a, float *b){     //Dot Product 3-Vectors
  float r = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
  return r;
}


float ray_tracing(float *r, float *s, float radius) {
  float A = dot3(r,r);
  float A_Min = getMinWithFMA(r, r);
  float A_Max = getMaxWithFMA(r, r);

  float B1 = dot3(s,r);   
  float B1_Min = getMinWithFMA(s, r);
  float B1_Max = getMaxWithFMA(s, r);                   
  float B = -2.0 * B1; 
  float B_Min  = getMinInvMult(-2.0, -2.0, B1_Min, B1_Max);
  float B_Max  = getMaxInvMult(-2.0, -2.0, B1_Min, B1_Max);

  float C1 = dot3(s,s);  
  float C1_Min = getMinWithFMA(r, r);
  float C1_Max = getMaxWithFMA(r, r);            
  float C = C1 - radius;
  float C_Min  = getMinInvMinus(C1_Min, C1_Max, radius, radius);
  float C_Max  = getMaxInvMinus(C1_Min, C1_Max, radius, radius);

  float D1 = B*B;
  float D1_Min  = getMinInvMult(B_Min, B_Max, B_Min, B_Max);
  float D1_Max  = getMaxInvMult(B_Min, B_Max, B_Min, B_Max);

  float D2 = A*C;
  float D2_Min  = getMinInvMult(A_Min, A_Max, C_Min, C_Max);
  float D2_Max  = getMaxInvMult(A_Min, A_Max, C_Min, C_Max);

  float D3 = 4 * D2;
  float D3_Min  = getMinInvMult(4, 4, D2_Min, D2_Max);
  float D3_Max  = getMaxInvMult(4, 4, D2_Min, D2_Max);

  float D = D1 - D3;
  float D_Min  = getMinInvMinus(D1_Min, D1_Max, D3_Min, D3_Max);
  float D_Max  = getMaxInvMinus(D1_Min, D1_Max, D3_Min, D3_Max);

  //self assign
  r[0] = dot3(r,r);
  r0_Min = getMinWithFMA(r, r);
  r0_Max = getMaxWithFMA(r, r);

  //return value           
  return D; //return D_Min, D_Max
} 

int main() {
  float r[3], s[3];
  float radius;
  FILE* fh;
  
  fh = fopen("r.txt", "r");
  //check if file exists
  if (fh == NULL){
    printf("file does not exists %s", "r.txt");
    return 0;
  }

r[0] = 4.021484375000000;
r[1] = -6.000000000000000;
r[2] = -5.000000000000000;
s[0] = 3.288188695907593;
s[1] = -5.000000000000000;
s[2] = -5.000000000000000; 
radius =  0.500000000000000;
ray_tracing(r, s, radius);

/*
int k = 0;
for (k = 0; k < 408; k++){
	fscanf(fh, "%f %f %f %f %f %f %f",
	&s[0],
	&s[1],
	&s[2],
	&r[0],
	&r[1],
	&r[2],      
	&radius);
	ray_tracing(r, s, radius);
}*/
  return 0;  
} 

