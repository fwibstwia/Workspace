int main(){
  float rx, ry, rz, sx, sy, sz;
  float A1 = (rx*rx + ry*ry) + rz*rz;
  float A2 = rx*rx + (ry*ry + rz*rz);
  
  float B1 = (sx*rx + sy*ry) + sz*rz;
  float B2 = sx*rx + (sy*ry + sz*rz);

  float C1 = (sx*sx + sy*sy) + sz*sz;
  float C2 = sx*sx + (sy*sy + sz*sz);
  

  float D1 = B1*B1 - 4*A1*C1;
  float D2 = B2*B2 - 4*A2*C2;

  if ( D1 == 0 && D1 - D2 > 0.000001 && sx > 0 && sx < 10 && sy > 0 && sy < 10 
	&& sz > 0 && sz < 10 && rx > 0 && rx < 10 && ry > 0 && ry < 10 && rz > 0 && rz < 10){
    assert(D2 == D1);
  }
}
