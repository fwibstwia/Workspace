#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int ray_tracing(float *r, float *s, float radius) {
  float A = r[0] * r[0] + r[1] * r[1] + r[2] * r[2];                       
  float B = -2.0 * (s[0] * r[0] + s[1] * r[1] + s[2] * r[2]);               
  float C = (s[0] * s[0] + s[1] * s[1] + s[2] * s[2])- radius;          
  float D = B*B - 4*A*C;   
  printf("%.19f\n", D);  
          
  return 0;
} 

int main()
{
    FILE* fp = fopen("r.txt", "r");
if (fp == NULL) 
{
  	fprintf(stderr, "Can't open inputs file!\n");
	return 0;
}
	int k = 0;
	for (k = 0; k < 408; k++)
{
	 float r[3];
	 float s[3];
         float radius;

	fscanf(fp, "%f %f %f %f %f %f %f",
	&r[0],
	&r[1],
	&r[2],
	&s[0],
	&s[1],
	&s[2],      
	&radius);
	ray_tracing(r, s, radius);
}

fclose(fp);


	return 0;
}
