int main(){
  float rx, ry, rz, sx, sy, sz;
  float A1 = (rx*rx + ry*ry) + rz*rz;
  float A2 = rx*rx + (ry*ry + rz*rz);
  
  float B1 = (sx*rx + sy*ry) + sz*rz;
  float B2 = (sx*rx + sy*ry) + sz*rz;

  float C1 = (sx*sx + sy*sy) + sz*sz;
  float C2 = (sx*sx + sy*sy) + sz*sz;
  

  float D1 = B1*B1 - 4*A1*C1;
  float D2 = B1*B1 - 4*A2*C1;

  if ( D1 == 0){
    assert(D2 == D1);
  }
}
