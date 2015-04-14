#include <stdio.h>

void mandelbrot(double x, double y, double cx, double cy)
{
        int iteration;
        //int itermax = 100;              /* how many iterations to do    */
        //double magnify=1.0;             /* no magnification             */
  
        //cx = (((float)hx)/((float)hxres)-0.5)/magnify*3.0-0.7;
        //cy = (((float)hy)/((float)hyres)-0.5)/magnify*3.0;
        double xx;
        cx = -cx;
        for (iteration = 1;iteration < 100;iteration++)  {
	   double xx1 = y*y + cx;
           xx1 = -xx1;
           xx = x*x + xx1;
           double y1 = 2.0*x;
           y = y1*y+cy;
           x = xx;     
           printf("%d\n", iteration); 
           printf("%.19f\n", x*x + y*y);   
           if (x*x+y*y>100.0) iteration = 999999;     
        }   	
}

int main() {
  double x, y, cx, cy;
  x = 1.5817982857277523845; 
  y = 1.3300781250000000000; 
  cx = -2.1700000006705524314;    
  cy = -1.4400000013411045074;
  mandelbrot(x, y, cx, cy);
  return 0;
} 
