#include <stdio.h>
int main() {
double a = 0, b = -a;
double ra = 1/a, rb = 1/b;
if (a == b && ra != rb)
printf("Ho hum a=%f == b=%f but 1/a=%f != 1/b=%f\n", a,b, ra,rb);
return 0; }
