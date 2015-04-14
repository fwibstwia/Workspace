#include <stdio.h>
int sum(float *r) {
  float A = 0;  
  int i;
  for(i = 0; i < 32; i ++){
      A = A + r[i];
  }
  printf("%.19f\n", A);          
  if (A > 0)
    return 0;
} 
int main(){
  float r[32];
  FILE* fh;
  
  fh = fopen("r.txt", "r");
  //check if file exists
  if (fh == NULL){
    printf("file does not exists %s", "r.txt");
    return 0;
  }

int k = 0;
for (k = 0; k < 78; k++){
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
	sum(r);
}
}
