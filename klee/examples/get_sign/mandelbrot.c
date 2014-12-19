/**  mandel.c   by Eric R. Weeks   written 9-28-96
 **weeks@physics.emory.edu
 **http://www.physics.emory.edu/~weeks/
 **
 **  This program is public domain, but this header must be left intact
 **  and unchanged.
 **
 **  to compile:  cc -o mand mandel.c
 **
 **/

#include <klee/klee.h>

mandelbrot(double x, double y)
{
        double cond;
        int iteration;
        //int itermax = 100;              /* how many iterations to do    */
        //double magnify=1.0;             /* no magnification             */
  
        //cx = (((float)hx)/((float)hxres)-0.5)/magnify*3.0-0.7;
        //cy = (((float)hy)/((float)hyres)-0.5)/magnify*3.0;
        
        double cx = -2.1939999997150154698;
	double cy = -1.4939999997150152922;
         
        
        for (iteration = 1;iteration < 3;iteration++)  {
           cx = -cx;
	   double xx1 = y*y + cx;
	   klee_tag_reorderable(&xx1, 0, 3);
           xx1 = -xx1;
           double xx = x*x + xx1;
           klee_tag_reorderable(&xx, 0, 3);
           double y1 = 2.0*x;
           y = y1*y+cy;
           klee_tag_reorderable(&y, 0, 3);
           x = xx;          
        }
       
        cond = x*x + y*y; 
        klee_tag_reorderable(&cond, 0, 3);
	if (cond - 100.0 > 0)  
	  iteration = 999999;	
}

int main() {
  double x, y;
  klee_make_symbolic_with_sort(&x, sizeof(x), "x", 0, 64);
  klee_make_symbolic_with_sort(&y, sizeof(y), "y", 0, 64);
  return mandelbrot(x, y);
} 
