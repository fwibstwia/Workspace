#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mandelbrot(double cx, double cy)
{
  
        int iteration;
        //int itermax = 100;              /* how many iterations to do    */
        //double magnify=1.0;             /* no magnification             */
  
        //cx = (((float)hx)/((float)hxres)-0.5)/magnify*3.0-0.7;
        //cy = (((float)hy)/((float)hyres)-0.5)/magnify*3.0;
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

int main() {
  double x, y, cx, cy;
  FILE* fh;
  char * line = NULL;
  char *saveptr1, *token;
  size_t len = 0;
  ssize_t read;
  
  fh = fopen("r.txt", "r");
  //check if file exists
  if (fh == NULL){
    printf("file does not exists %s", "r.txt");
    return 0;
  }
  
  while ((read = getline(&line, &len, fh)) != -1) {
     token = strtok_r(line, " ", &saveptr1);
     cx = atof(token);
     line = NULL;
     token = strtok_r(line, " ", &saveptr1);
     cy = atof(token);
     line = NULL;
     mandelbrot(cx, cy);
  }
  return 0;
} 
