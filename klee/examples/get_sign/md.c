#include <klee/klee.h>
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
    klee_tag_reorderable(&rr, 0, 3);
  }
  if(rr > rrCut){
    return 0;
  }

}
int main() {
  double r_j1[3], r_j2[3];
  klee_make_symbolic_with_sort(&r_j1, sizeof(r_j1), "r_j1", 8, 64);
  klee_make_symbolic_with_sort(&r_j2, sizeof(r_j2), "r_j2", 8, 64);
  return ComputeAccel(r_j1, r_j2);
} 
