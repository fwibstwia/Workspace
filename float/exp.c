#include <stdio.h>
#include <stdlib.h>
#include <fenv.h>
#include <math.h>
#include <glib.h>
#include <mpfr.h>

#include "reorder.h"

void geneRandFloats(float* fs, int size){
  srand((unsigned int) time(NULL));

  for(int i = 0; i < size; i ++){
    fs[i] = MIN_RAND + (float)rand()/(float)(RAND_MAX) * (MAX_RAND - MIN_RAND);
  }
}

int main(int argc, char* argv[]){

}

