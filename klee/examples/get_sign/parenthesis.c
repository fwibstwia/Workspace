float non_parenthesis(float a, float b, float c, float d, float e, float f){
  return a + b + (c + d + e)*f;
}

float parenthesis(float a, float b, float c, float d, float e, float f){
  return a + b + ((c + d) + e)*f;
}
