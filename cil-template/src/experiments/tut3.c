
#include <stdio.h>

float dot3(float *a, float *b){     //Dot Product 3-Vectors
  float r = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
  return r;
}

int ray_tracing(float *r, float *s, float radius) {
  float A = dot3(r,r);                       
  float B = -2.0 * dot3(s,r);               
  float C = dot3(s,s) - radius;          
  float D = B*B - 4*A*C;  
  //float D = A + B - C ;*/ 
  printf("%.19f\n", D);  
           
  if (D > 0){
    //printf("1\n");  
  }else{
    //printf("-1\n"); 
  }
  return 0;
} 

int main()
{
  float A __attribute__((reorderable)), B __attribute__((reorderable)),
        C __attribute__((reorderable));
   
   



  return 0;
}
