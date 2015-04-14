/*
 * First KLEE tutorial: testing a small function
 */

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

