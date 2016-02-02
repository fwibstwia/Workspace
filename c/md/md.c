#include <stdio.h>
#define RCUT 2.5     /* Potential cut-off length */

double SignR(double v,double x) {if (x > 0) return v; else return -v;}

int ComputeAccel(double *r_j1, double *r_j2){
  double RegionH[3];/* Half the box lengths */
  double dr[3];
  double rr;
  double rrCut = RCUT*RCUT;
  int k;
  RegionH[0] = 11.8563110149668755611;
  RegionH[1] = 11.8563110149668755611;
  RegionH[2] = 11.8563110149668755611;
  for (rr=0.0, k=0; k<3; k++) {
    dr[k] = r_j1[k] - r_j2[k];
    /* Chooses the nearest image */
    dr[k] = dr[k] - SignR(RegionH[k],dr[k]-RegionH[k])
      - SignR(RegionH[k],dr[k]+RegionH[k]);
    rr = dr[k]*dr[k] + rr;
  }

  printf("%.19f\n", rr);
  return 0;
}
int main() {
  double r_j1[3], r_j2[3];
  r_j1[0]=-2.446298223847615844889;
  r_j1[1]=-23.212622029933751122;
  r_j1[2]=23.587622029933751122;
  r_j2[0]=0;
  r_j2[1]=0;
  r_j2[2]=0;
  return ComputeAccel(r_j1, r_j2);
} 
