#include "Mandelbrot.h"

double mandelbrot_nonfma(double x0, double y0, double windowPosX, double windowPosY, double stepSize){
  double wsX = windowPosX*stepSize;
  double wsY = windowPosY*stepSize;
  double stepPosX = x0 + wsX;
  double stepPosY = y0 - wsY;

  double stepPosX_sqr = stepPosX * stepPosX;
  double stepPosY_sqr = stepPosY * stepPosY;

  double x =  stepPosX_sqr - stepPosY_sqr + stepPosX;
  double y =  2 * stepPosX * stepPosY + stepPosY;

  double x_sqr = x*x;
  double y_sqr = y*y;

  return x_sqr + y_sqr; 
}
