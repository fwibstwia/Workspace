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
  float D = B*B - 4.*(A*C);
  printf("%.18f\n", D);  
           
  if (D > 0)
    return 0;
} 

main() 
{
  float r[3], s[3];
  float radius;
  FILE* fp = fopen("inputs", "r");
  int i;
  for (i = 0; i < 408; i++)
  {
    fscanf(fp, "%f %f %f %f %f %f %f", &s[0], &s[1], &s[2], &r[0], &r[1], &r[2], &radius);
    ray_tracing(r, s, radius);
  }
  fclose(fp);
} 

