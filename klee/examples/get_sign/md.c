#include <klee/klee.h>
#define RCUT 0.5     /* Potential cut-off length */

float SignR(float v,float x) {if (x > 0) return v; else return -v;}

int ComputeAccel(float *r_j1, float *r_j2){
  float RegionH = 17.0997600555419921875f;/* Half the box lengths */
  float dr[3];
  float rr = 0;
  float rrCut = RCUT*RCUT;
  int k;
  if((r_j1[0] > 0) && (r_j1[1] > 0) && (r_j1[2] > 0)
	      && (r_j2[0] > 0) && (r_j2[1] > 0) && (r_j2[2] > 0)){
  for (k=0; k<3; k++) {
    dr[k] = r_j1[k] - r_j2[k];
    /* Chooses the nearest image */
    dr[k] = dr[k] - SignR(RegionH,dr[k]- RegionH)
      - SignR(RegionH,dr[k]+RegionH);
    rr = dr[k]*dr[k] + rr;
    klee_tag_reorderable(&rr, 0, 3);
  }
  if(rr < rrCut){
    return 0;
  }
 }

}
int main() {
  float r_j1[3], r_j2[3];
  klee_make_symbolic_with_sort(&r_j1, sizeof(r_j1), "r_j1", 8, 32);
  klee_make_symbolic_with_sort(&r_j2, sizeof(r_j2), "r_j2", 8, 32);
  return ComputeAccel(r_j1, r_j2);
} 
