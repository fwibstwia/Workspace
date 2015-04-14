#include <stdio.h>
#include <math.h>
#include <time.h>

#define RCUT 0.5     /* Potential cut-off length */

float Region[3];  /* MD box lengths */
float RegionH[3]; /* Half the box lengths */
int InitUcell[3] = {10, 10, 10};   /* Number of unit cells */


float SignR(float v,float x) {if (x > 0) return v; else return -v;}

void InitParams() {
  int k;
  float Density = 0.1;
  for (k=0; k<3; k++) {
      Region[k] = InitUcell[k]/pow(Density/4.0,1.0/3.0);
      RegionH[k] = 0.5*Region[k];
  }
  printf("%.19f %.19f %.19f", RegionH[0], RegionH[1], RegionH[2]);     
}

void ComputeAccel(float *r_j1, float *r_j2){
  float dr[3];
  float rr, rrCut;
  rrCut = RCUT*RCUT;
  int k;

  for (rr=0.0, k=0; k<3; k++) {
    dr[k] = r_j1[k] - r_j2[k];
    /* Chooses the nearest image */
    dr[k] = dr[k] - SignR(RegionH[k],dr[k]-RegionH[k])
      - SignR(RegionH[k],dr[k]+RegionH[k]);
    rr = rr + dr[k]*dr[k];
  }
 printf("rr: %.19f\n", rr);
  if(rr < rrCut){
    printf("1\n");
    return;
  }
  printf("0\n");
}

int main() {
  float r_j1[3];
  float r_j2[3];
  FILE* fh;

  //mandelbrot_int(269,152);
   
  fh = fopen("r.txt", "r");
  //check if file exists
  if (fh == NULL){
    printf("file does not exists %s", "r.txt");
    return 0;
  }
  InitParams();  
  int k = 0;
  for (k = 0; k < 7; k++){
     fscanf(fh, "%f %f %f %f %f %f",
	       &r_j2[0], &r_j2[1], &r_j2[2],&r_j1[0], &r_j1[1], &r_j1[2]);
    ComputeAccel(r_j1, r_j2); /* Computes initial accelerations */ 
  }
  
  return 0;
} 

