#include <stdio.h>
int main(){
double v = 1E308;
double x = (v * v) / v;
printf("%g %d\n", x, x==v);
}
