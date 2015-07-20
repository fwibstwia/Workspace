#include <stdio.h>

#define COUNT 32

int choice[COUNT][COUNT];
float value[COUNT][COUNT];
float r[COUNT];

void getREBoundF(int direction, int len){
  float c, t;
  for(int i = 0; i < len; i ++){
    value[i][i] = r[i];
  }

  for(int l = 2; l <= len; l ++){
    for(int i = 0; i < len - l + 1; i ++){
      int j = i + l - 1;
      choice[i][j] = i;
      c = value[i][i] + value[i+1][j];
      value[i][j] = value[i][i] + value[i+1][j];
      for(int k = i + 1;  k < j; k++){
	t = value[i][k] + value[k+1][j];
	if(direction == 1){
	  if(t > c){
	    c = t;
	    choice[i][j] = k;
            value[i][j] = value[i][k] + value[k+1][j];
	  }
	}else{
	  if(t < c){
	    c = t;
	    choice[i][j] = k;
            value[i][j] = value[i][k] + value[k+1][j];
	  }
	}
      }
    }
  }
}

float sum() {
  float A = 0;
  int i;
  for(i = 0; i < 32; i ++){
    A = A + r[i];
  }
  return A;
}

int main(){
  FILE* fh;

  fh = fopen("r.txt", "r");
  //check if file exists
  if (fh == NULL){
    printf("file does not exists %s", "r.txt");
    return 0;
  }

  int k = 0;
  for (k = 0; k < 100; k++){
    fscanf(fh,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
           &r[0],
           &r[1],
           &r[2],
           &r[3],
           &r[4],
           &r[5],
           &r[6],
           &r[7],
           &r[8],
           &r[9],
           &r[10],
           &r[11],
           &r[12],
           &r[13],
           &r[14],
           &r[15],
           &r[16],
           &r[17],
           &r[18],
           &r[19],
           &r[20],
           &r[21],
           &r[22],
           &r[23],
           &r[24],
           &r[25],
           &r[26],
           &r[27],
           &r[28],
           &r[29],
           &r[30],
           &r[31]);
    float serial_sum = sum();
    getREBoundF(0, COUNT);
    float min_sum = value[0][COUNT - 1];
    getREBoundF(1, COUNT);
    float max_sum = value[0][COUNT - 1];
    //printf("%.19f\n", serial_sum);
    //printf("%.19f\n", min_sum);
    printf("%.19f\n", max_sum);
  }
}
