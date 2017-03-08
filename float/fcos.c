#include <stdio.h>
#include <math.h>

float fcos(float x){
  float r;
  r = 1 - x*x/2 + x*x*x*x/24;
  return r;
}

int main(){
  float input = -0.5;
  FILE *fp;
  fp=fopen("r.txt", "w");

  while (input < 0.5){
    if(input != 0){
      float y = (1 - fcos(input))/(input * input);
      fprintf(fp, "%.9f\n", y);
    }
    input = nextafter(input, 0.5);    
  }
  fclose(fp);
}
