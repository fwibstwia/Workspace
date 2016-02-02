int main(){
  float r1, r2, r3,r4,r5,r6,r7,r8,r9;
  float A1 = (r1*r1  + r2*r2) + r3*r3;
  float A2 = r1*r1 + (r2*r2 + r3*r3);
  if(r1 > 1 && r1 < 20 && r2 > 1 && r2 < 10 && r3 > 1 && r3 < 10 && 
     r4 > 1 && r4 < 10 && r5 > 1 && r5 < 10 && r6 > 1 && r6 < 10)
    assert(A1 == A2);
}
