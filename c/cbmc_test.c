int main(){
  float x1, y1, x2, y2, x3, y3;
  float q1 = (x1*x1 + x2*x2) + x3*x3;
  float q2 = x1*x1 + (x2*x2 + x3*x3);
  if ( q1 - q2 > 0.000005 && x1 > 10 && x1 < 20 && x2 > 10 && x2 < 20 && x3 > 100 && x3 < 200){
    assert(q2 == q1);
  }
}
