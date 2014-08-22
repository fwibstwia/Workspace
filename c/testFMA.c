#include <stdio.h>

float testFMAOrder(float a, float b, float c, float d, float e, float f){
  float g,h;
  g = a * b + c * d;
  h = e * f + a * b;
  return g + h;
}

