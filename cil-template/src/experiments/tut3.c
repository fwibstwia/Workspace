
#include <stdio.h>

int main()
{
  float a __attribute__((reorderable)),b,c;
  if (b - a < 0.1){
     a = a + b;
  }else{
     a = a + b;
  }
  a = a + b;
  return 0;
}
