#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mandelbrot(double cx, double cy)
{
  
        int iteration;
        double xx;
        double x = 0.0;
	double y = 0.0;
        for (iteration = 1;iteration < 100;iteration++)  {
	   xx = x*x-y*y+cx;
           y = 2.0*x*y+cy;
           x = xx;    

           if (x*x+y*y>100.0) break;     
        } 
           printf("%d\n", iteration); 
           //printf("%.19f\n", x*x + y*y); 
          	
}
