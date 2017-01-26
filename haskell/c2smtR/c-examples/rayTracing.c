int main(){
float rx, ry, rz, sx, sy, sz, radius;
float A = rx*rx+ry*ry+rz*rz;
float B = -2.0 * (sx*rx+sy*ry+sz*rz);
float C = sx*sx+sy*sy+sz*sz - radius;
float D = B*B - 4.0*A*C;                   
if (D > 0.0){
  return 0;
}
}
