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

mandelbrot(float cx, float cy)
{
        float cond;
        float x,y; //cx, cy;
        int iteration;
        //int itermax = 100;              /* how many iterations to do    */
        //float magnify=1.0;             /* no magnification             */
  
        //cx = (windowPosX/500.0-0.5)/1.0*3.0-0.7;
        //cy = (windowPosY/500.0-0.5)/1.0*3.0;
        
        x = 0.0;
        y = 0.0; 
        
        for (iteration = 1;iteration < 6;iteration++)  {
	   float xx1 = y*y + cx;
	   klee_tag_reorderable(&xx1, 0, 3);
           xx1 = -xx1;
           float xx = x*x + xx1;
           klee_tag_reorderable(&xx, 0, 3);
           float y1 = 2.0*x;
           y = y1*y+cy;
           klee_tag_reorderable(&y, 0, 3);
           x = xx;          
        }
       
        cond = x*x + y*y; 
        klee_tag_reorderable(&cond, 0, 3);
	if (cond > 1.0)  
	  iteration = 999999;	
}

int main() {
  float windowPosX, windowPosY;
  klee_make_symbolic_with_sort(&windowPosX, sizeof(windowPosX), "windowPosX", 0, 32);
  klee_make_symbolic_with_sort(&windowPosY, sizeof(windowPosY), "windowPosY", 0, 32);
  return mandelbrot(windowPosX, windowPosY);
} 
