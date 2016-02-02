float dot3(float *a, float *b){     //Dot Product 3-Vectors
  float r;
  r = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
  return r;
}

int raySphere(float *r, float *s, float radius) {
  float A = dot3(r,r);                       
  float B = -2.0 * dot3(s,r);               
  float C = dot3(s,s) - radius;          
  float D = B*B - 4*A*C;                       
  if (D > 0)
    return 0;
} 

int main() {
  float r[3], s[3];
  float radius;
  return raySphere(r, s, radius);
} 

