#include <stdio.h>
#include <math.h>
float sum3_right_asso(float a, float b, float c){
  return a + (b + c);
}
float sum3_left_asso(float a, float b, float c){
  return (a + b) + c;
}

int main(){
  float a = 1.0f, b = 1.0f, c = 1.0f;
  int i = 0;
  FILE *fh;
  fh = fopen("r.txt", "w");
 
  if(fh == NULL){
    return 0;
  }
  
  //degenerate case: a = point; b = point; c = interval;
  float prev_sr = 0.0f, prev_sl = 0.0f;
  while(c <= 1.1){
    float sr = sum3_right_asso(a, b, c);
    float sl = sum3_left_asso(a, b, c);
    //	if(sl != sr){
    fprintf(fh, "%d %.17f %.17f\n", i++, sl, sr);
    //}
    c = nextafterf(c, 100);
  }
  /*
  while(a <= 10.1){
    while(b <= 1.1){
      while(c <= 1.1){
	float sr = sum3_right_asso(a, b, c);
	float sl = sum3_left_asso(a, b, c);
	//	if(sl != sr){
	fprintf(fh, "%d %.17f %.17f\n", i++, sl, sr);
	  //}
	c = nextafterf(c, 100);
      }
      b = nextafterf(b, 100);
    }
    a = nextafterf(a, 100);
    } */
}
