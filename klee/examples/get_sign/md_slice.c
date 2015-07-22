#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <klee/klee.h>

#define NMAX 100000  /* Maximum number of atoms which can be simulated */
#define RCUT 2.5     /* Potential cut-off length */
#define PI 3.141592653589793
/* Constants for the random number generator */
#define D2P31M 2147483647.0
#define DMUL 16807.0

void InitParams();
void InitConf();
void ComputeAccel();
/* Input parameters (read from an input file in this order) *******************/

int InitUcell[3];   /* Number of unit cells */
float Density;     /* Number density of atoms (in reduced unit) */
float DeltaT;      /* Size of a time step (in reduced unit) */


/* Constants ******************************************************************/

float Region[3];  /* MD box lengths */
float RegionH[3]; /* Half the box lengths */
float DeltaTH;    /* Half the time step */


/* Variables ******************************************************************/

int nAtom;            /* Number of atoms */
float r[NMAX][3];    /* r[i][0|1|2] is the x|y|z coordinate of atom i */
float rv[NMAX][3];   /* Atomic velocities */
float ra[NMAX][3];   /* Acceleration on atoms */

/******************************************************************************/

float SignR(float v,float x) {
  if (x > 0) return v;
  else return -v;
}


int main() {
  klee_make_symbolic_with_sort(&Region, sizeof(Region), "Region", 8, 32);
  InitParams();
  InitConf();
  ComputeAccel();
  return 0;
}

/*----------------------------------------------------------------------------*/
void InitParams() {
  /*------------------------------------------------------------------------------
    Initializes parameters.
    ------------------------------------------------------------------------------*/
  int k;
  /* Reads control parameters */
  /*scanf("%d%d%d",&InitUcell[0],&InitUcell[1],&InitUcell[2]);
    scanf("%le",&Density);
    scanf("%le",&InitTemp);
    scanf("%le",&DeltaT);
    scanf("%d",&StepLimit);
    scanf("%d",&StepAvg);*/

  InitUcell[0]=10;
  InitUcell[1]=10;
  InitUcell[2]=10;

  //Density=((float)rand()/(float)RAND_MAX);

  /* Computes basic parameters */
  for (k=0; k<3; k++) {
    //Region[k] = InitUcell[k]/pow(Density/4.0,1.0/3.0);
    RegionH[k] = 0.5*Region[k];
  }
}

/*----------------------------------------------------------------------------*/
void InitConf() {
  /*------------------------------------------------------------------------------
    r are initialized to face-centered cubic (fcc) lattice positions.
    rv are initialized with a random velocity corresponding to Temperature.
    ------------------------------------------------------------------------------*/
  float c[3],gap[3];
  int j,k,nX,nY,nZ;

  float origAtom[4][3] = {{0.0, 0.0, 0.0}, {0.0, 0.5, 0.5},
                          {0.5, 0.0, 0.5}, {0.5, 0.5, 0.0}};

  /* Sets up a face-centered cubic (fcc) lattice */
  for (k=0; k<3; k++) gap[k] = Region[k]/InitUcell[k];
  nAtom = 0;
  for (nZ=0; nZ<InitUcell[2]; nZ++) {
    c[2] = nZ*gap[2];
    for (nY=0; nY<InitUcell[1]; nY++) {
      c[1] = nY*gap[1];
      for (nX=0; nX<InitUcell[0]; nX++) {
        c[0] = nX*gap[0];
        for (j=0; j<4; j++) {
          for (k=0; k<3; k++){
            r[nAtom][k] = c[k] + gap[k]*origAtom[j][k];
          }
          ++nAtom;
        }
      }
    }
  }
}

void ComputeAccel(){
  float dr[3],rrCut,rr;
  int j1,j2,k;
  float signal;
  rrCut = RCUT*RCUT;

  /* Doubly-nested loop over atomic pairs */
  for (j1=0; j1<nAtom-1; j1++) {
    for (j2=j1+1; j2<nAtom; j2++) {
      /* Computes the squared atomic distance */
      for (rr=0.0, k=0; k<3; k++) {
        dr[k] = r[j1][k] - r[j2][k];
        /* Chooses the nearest image */

        dr[k] = dr[k] - SignR(RegionH[k],dr[k]-RegionH[k])
          - SignR(RegionH[k],dr[k]+RegionH[k]);
        rr = rr + dr[k]*dr[k];
      }

      if(rr < rrCut){
          signal = 1.0;
      }
    }
  }
}
