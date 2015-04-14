#include <stdio.h>
#include <stdlib.h>
#include <string.h>
double f_orientation(double* p, double* q, double* r) {
    double det = (q[0]-p[0])*(r[1]-p[1]) - (q[1]-p[1])*(r[0]-p[0]);
    printf("det: %.19f\n", det);
    return det;
}

double f_extended(double* p, double* q, double* r) {
     double l = (q[0]-p[0]) * (r[0]-q[0]) + (q[1] - p[1]) * (r[1]-q[1]);
     printf("l: %.19f\n", l);
     return l;
}

// extended_rightturn: Returns true if pqr form a rightturn, or if pqr are 
// collinear and if r is on the extension of the ray starting in q in
// the direction q-p, i.e., if (q-p)*(r-q) >= 0.
// All points must be from the range [first,last), which will be used
// for computing indices in the verbose trace output.
int extended_rightturn(double* p, double* q, double* r) {
    double det = f_orientation(p,q,r);
    if(det < 0.0 || (det == 0.0 && f_extended(p,q,r) >= 0.0)){
	printf("1\n");
    }else{
     printf("0\n");
    }
    return 0;
}

int area_sign(float* a, float* b, float* c, float *d) {
    float ax, ay, az, bx, by, bz, cx, cy, cz;
    float vol;
    ax = a[0] - d[0];
    ay = a[1] - d[1];
    az = a[2] - d[2];
    bx = b[0] - d[0];
    by = b[1] - d[1];
    bz = b[2] - d[2];
    cx = c[0] - d[0];
    cy = c[1] - d[1];
    cz = c[2] - d[2];

    float yz = by*cz + (-bz)*cy;
    float zx = bz*cx + (-bx)*cz;
    float xy = bx*cy + (-by)*cx;

    vol = ax*yz + ay*zx + az*xy;
    printf("%.19f\n", vol);
    if(vol > 0.5)
       printf("1\n");
    else printf("0\n");
    return 0;
}

int main() {
  double p[2], q[2], r[2];

  float a[3], b[3], c[3], d[3];
  FILE* fh;   
  fh = fopen("r.txt", "r");
  //check if file exists
  if (fh == NULL){
    printf("file does not exists %s", "r.txt");
    return 0;
  }
  int k = 0;
  for (k = 0; k < 20; k++){
     fscanf(fh, "%f %f %f %f %f %f %f %f %f %f %f %f",
	       &a[0], &a[1], &a[2], 
               &d[0], &d[1], &d[2], 
               &b[0], &b[1], &b[2], 
               &c[0], &c[1], &c[2]);
     area_sign(a, b, c, d);
  }
  /*
  int k = 0;
  for (k = 0; k < 10; k++){
     fscanf(fh, "%lf %lf %lf %lf %lf %lf",
	       &q[0], &q[1], &p[0], &p[1], &r[0], &r[1]);
     extended_rightturn(p, q, r);
  }*/
  return 0;
}
