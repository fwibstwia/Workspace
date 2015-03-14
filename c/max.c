#include <stdio.h>
#include <math.h>
void cal(float sx, float sy, float sz){
  float r = nextafterf(0.0, 2.0);
  printf("%.23f\n", r);
}
int main(){
  float sx = 0.005782f ;
  float sy = 31.999878f   ;
  float sz = 0.111744f;
  cal(sx,sy,sz);
}
