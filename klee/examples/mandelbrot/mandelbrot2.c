#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mandelbrot(float cx, float cy)
{
        int iteration;
        float xx;
        float x = 0.0;
	float y = 0.0;

        for (iteration = 1;iteration < 100;iteration++)  {
	   xx = x*x-y*y+cx;
           y = 2.0*x*y+cy;
           x = xx;    
           //printf("%.19f\n", x*x + y*y); 
           if (x*x+y*y>100) break;     
        }
 
        printf("%d\n", iteration);         	
}

void mandelbrot_fix_iter(float cx, float cy){
        int iteration;
        float xx;
        float x = 0.0;
	float y = 0.0;

        for (iteration = 1;iteration < 60;iteration++)  {
	   xx = x*x-y*y+cx;
           y = 2.0*x*y+cy;
           x = xx;    
           printf("%.19f\n", x*x + y*y);             
        }
        printf("iterations: %d\n", iteration);
        printf("%.19f\n", x*x + y*y); 
        if (x*x+y*y>1.0){
          printf("%d\n", 1); 
        }else{
          printf("%d\n", 0);
        }
        

}


void mandelbrot_int(int windowPosX, int windowPosY){
        int iteration;
        int itermax = 100;              /* how many iterations to do    */
        float MAGNIFY=1.0;             /* no magnification             */
        int X_SIZE = 500;
	int Y_SIZE = 500;
 
        float cx = (((float)windowPosX+1)/((float)X_SIZE)-0.5)/MAGNIFY*3.0-0.7;
        float cy = (((float)windowPosY+1)/((float)Y_SIZE)-0.5)/MAGNIFY*3.0;

        printf("%.19f\n", cx);
        printf("%.19f\n", cy);
  
        float xx;
        float x = 0.0;
	float y = 0.0;
        for (iteration = 1;iteration < 100;iteration++)  {
	   xx = x*x-y*y+cx;
           y = 2.0*x*y+cy;
           x = xx;    
           printf("%.19f\n", x*x + y*y); 
           if (x*x+y*y>100) break;     
        } 
        printf("%d\n", iteration); 
}

int main() {
  float cx, cy;
  /*
  FILE* fh;

  //mandelbrot_int(269,152);
   
  fh = fopen("r.txt", "r");
  //check if file exists
  if (fh == NULL){
    printf("file does not exists %s", "r.txt");
    return 0;
  }

  int k = 0;
  for (k = 0; k < 35; k++){
     fscanf(fh, "%f %f",
	       &cx, &cy);
     mandelbrot(cx, cy);
  }*/
  mandelbrot_fix_iter(0.2799655348062515, 0.01);
  return 0;
} 
