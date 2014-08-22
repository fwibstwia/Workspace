#include <stdio.h>
#include <mpfr.h>
int main(){
  int precision = 1024;
  mpfr_t rx,ry,rz,sx,sy,sz,radius, temp1, temp2,temp3,A,B,C,D,cons, cons4;
  mpfr_init2(rx, precision);
  mpfr_init2(ry, precision);
  mpfr_init2(rz, precision);
  mpfr_init2(sx, precision);
  mpfr_init2(sy, precision);
  mpfr_init2(sz, precision);
  mpfr_init2(temp1, precision);
  mpfr_init2(temp2, precision);
  mpfr_init2(temp3, precision);
  mpfr_init2(A, precision);
  mpfr_init2(B, precision);
  mpfr_init2(C, precision);
  mpfr_init2(D, precision);
  mpfr_init2(cons, precision);
  mpfr_init2(cons4, precision);
  mpfr_init2(radius, precision);
  mpfr_set_flt(rx, 0.125, MPFR_RNDN);
  mpfr_set_flt(ry, -1.0450602, MPFR_RNDN);
  mpfr_set_flt(rz, 3.2025185, MPFR_RNDN);
  mpfr_set_flt(sx, -1.6350498, MPFR_RNDN);
  mpfr_set_flt(sy, 4.8326674, MPFR_RNDN);
  mpfr_set_flt(sz, -3.631185, MPFR_RNDN);
  mpfr_set_flt(cons, -2, MPFR_RNDN);
  mpfr_set_flt(cons4, -4, MPFR_RNDN);
  mpfr_set_flt(radius, -14.128791, MPFR_RNDN);
  mpfr_mul(temp1, rx,rx, MPFR_RNDN);
  mpfr_mul(temp2, ry,ry, MPFR_RNDN);
  mpfr_mul(temp3, rz,rz, MPFR_RNDN);
  mpfr_add(temp1, temp1, temp2, MPFR_RNDN);
  mpfr_add(A, temp1, temp3, MPFR_RNDN);
  mpfr_mul(temp1, rx,sx, MPFR_RNDN);
  mpfr_mul(temp2, ry,sy, MPFR_RNDN);
  mpfr_mul(temp3, rz,sz, MPFR_RNDN);
  mpfr_add(temp1, temp1, temp2, MPFR_RNDN);
  mpfr_add(temp1, temp1, temp3,MPFR_RNDN);
  mpfr_mul(B, cons, temp1, MPFR_RNDN);
  mpfr_mul(temp1, sx,sx, MPFR_RNDN);
  mpfr_mul(temp2, sy,sy, MPFR_RNDN);
  mpfr_mul(temp3, sz,sz, MPFR_RNDN);
  mpfr_add(temp1, temp1, temp2, MPFR_RNDN);
  mpfr_add(temp1, temp1, temp3,MPFR_RNDN);
  mpfr_add(C, temp1, radius, MPFR_RNDN);
  mpfr_mul(temp1, B, B, MPFR_RNDN);
  mpfr_mul(temp2, A, C, MPFR_RNDN);
  mpfr_mul(temp2, temp2, cons4, MPFR_RNDN);
  mpfr_add(D, temp1, temp2, MPFR_RNDN);
  mpfr_printf ("%.512Rf\n", A);
  mpfr_printf ("%.512Rf\n", B);
  mpfr_printf ("%.512Rf\n", C);
  mpfr_printf ("%.512Rf\n", D);

} 
