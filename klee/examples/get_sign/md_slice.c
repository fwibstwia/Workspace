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
void EvalProps();
void ApplyBoundaryCond();
void HalfKick(int stepcount,int sel);
void SingleStep(int stepcount);

int InitUcell[3];   /* Number of unit cells */
float Density;     /* Number density of atoms (in reduced unit) */
float InitTemp;    /* Starting temperature (in reduced unit) */
float DeltaT;      /* Size of a time step (in reduced unit) */
int StepLimit;      /* Number of time steps to be simulated */

float Region[3];  /* MD box lengths */
float RegionH[3]; /* Half the box lengths */
float DeltaTH;    /* Half the time step */
float Duc;    /* Potential cut-off parameters */

int nAtom;            /* Number of atoms */
float r[NMAX][3];    /* r[i][0|1|2] is the x|y|z coordinate of atom i */
float rv[NMAX][3];   /* Atomic velocities */
float ra[NMAX][3];   /* Acceleration on atoms */
int stepCount;        /* Current time step */

float e[3];


float SignR(float v,float x) {if (x > 0) return v; else return -v;}
int main() {
  klee_make_symbolic_with_sort(&e, sizeof(e), "element", 8, 32);
  InitParams();
  InitConf();
  ComputeAccel();

  /*
    for (stepCount=1; stepCount<=StepLimit; stepCount++) {
    SingleStep(stepCount);
    }*/
  return 0;

}
void InitParams() {
  int k;
  float rr,ri2,ri6,r1;

  InitUcell[0]=10;
  InitUcell[1]=10;
  InitUcell[2]=10;

  Density=0.5;//((float)rand()/(float)RAND_MAX);

  InitTemp=1.0;
  DeltaT=0.005;
  StepLimit=10;
  /* Computes basic parameters */
  DeltaTH = 0.5*DeltaT;
  for (k=0; k<3; k++) {
    Region[k] = InitUcell[k]; ///pow(Density/4.0,1.0/3.0);
    RegionH[k] = 0.5*Region[k];
  }

  /* Constants for potential truncation */
  rr = RCUT*RCUT; ri2 = 1.0/rr; ri6 = ri2*ri2*ri2; r1=rr;//sqrt(rr);
  //Duc = -48.0*ri6*(ri6 - 0.5)/r1;
}

/*----------------------------------------------------------------------------*/
void InitConf() {
  /*------------------------------------------------------------------------------
    r are initialized to face-centered cubic (fcc) lattice positions.
    rv are initialized with a random velocity corresponding to Temperature.
    ------------------------------------------------------------------------------*/
  float c[3],gap[3],vSum[3],vMag;
  int j,n,k,nX,nY,nZ;
  float seed;
  /* FCC atoms in the original unit cell */
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


  /* Generates random velocities */
  seed = 13597.0;
  vMag = 2;//sqrt(3*InitTemp);
  for(k=0; k<3; k++) vSum[k] = 0.0;
  for(n=0; n<nAtom; n++) {
    //RandVec3(e,&seed);
    for (k=0; k<3; k++) {
      rv[n][k] = vMag*e[k];
      vSum[k] = vSum[k] + rv[n][k];
    }
  }
  /* Makes the total momentum zero */
  for (k=0; k<3; k++) vSum[k] = vSum[k]/nAtom;
  for (n=0; n<nAtom; n++) for(k=0; k<3; k++) {
      rv[n][k] = rv[n][k] - vSum[k];
    }
}



void ComputeAccel(){
  float dr[3],f,fcVal,rrCut,rr,ri2,ri6,r1;
  int j1,j2,n,k;

  rrCut = RCUT*RCUT;
  for (n=0; n<nAtom; n++) for (k=0; k<3; k++) ra[n][k] = 0.0;

  dr[0] = r[0][0] - r[0][0];
        /* Chooses the nearest image */

  dr[1] = dr[1] - SignR(RegionH[0],dr[1]-RegionH[0])
          - SignR(RegionH[0],dr[1]+RegionH[0]);
  rr = rr + dr[1]*dr[1];
  if(rr < rrCut){
      rr = rrCut;
  }
}
