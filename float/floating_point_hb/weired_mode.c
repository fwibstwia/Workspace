#include <stdio.h>

int weiredMode(int n){
  if(n == 1)
    return 1;
  int k = weiredMode(n-1);
  return k + k%(2*n);
}
int main(){
  int k = weiredMode(396);
  printf("%d", k);
}
