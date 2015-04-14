#include <stdio.h>

#define MAGNIFY	1.0
#define X_SIZE	500
#define Y_SIZE	500

main()
{
        float x,xx,y,cx,cy;
        int iteration, hx, hy;
        for (hy = 1; hy <= Y_SIZE; hy++)  
	{
                for (hx = 1; hx <= X_SIZE; hx++)  
		{
                        cx = (((float)hx)/((float)X_SIZE)-0.5)/MAGNIFY*3.0-0.7;
                        cy = (((float)hy)/((float)Y_SIZE)-0.5)/MAGNIFY*3.0;
                        x = 0.0; y = 0.0;
                        for (iteration = 1; iteration < 100; iteration++)  
			{
                                xx = x*x-y*y+cx;
                                y = 2.0*x*y+cy;
                                x = xx;
                                if (x*x+y*y > 100.0)  break;
                        }
                        printf("%X\n", iteration);
                }
        }
}
