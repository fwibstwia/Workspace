#include <stdio.h>

static inline double f(double x) {
  return x/1E308;
}

double square(double x) {
  double y = x*x;
  return y;
}

int main(void) {
  printf("%g\n", f(square(1E308)));
}
