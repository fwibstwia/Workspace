#pragma STDC FP_CONTRACT ON

float f(float a, float b, float c){
  float g = a * b + c;
  return g;
}

int main(){
  float a, b, c;
  f(a, b, c);
  return 0;
}
