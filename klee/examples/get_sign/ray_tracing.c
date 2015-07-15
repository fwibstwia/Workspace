#include <stdlib.h>
#include <klee/klee.h>

/*
#include <stdio.h>
#include <math.h>
#include <algorithm> 
#include<iostream>

#include <time.h>
using namespace std;*/

float *sub3(float *a, float *b);
float *normalize3(float *v);
float dot3(float *a,float  *b);
float *mul3c(float* a, float c);
void raySphere(int idx, float *r, float *o);
void rayObject(int type, int idx, float *r, float *o);
void raytrace(float *ray, float *origin);

int nrTypes = 2;
int nrObjects [2] = {2,5};
float gOrigin [3] = {0.0,0.0,0.0};
float spheres [2][4] = {{1.0,0.0,4.0,0.5},{-0.6,-1.0,4.5,0.5}};        
float planes [5][2] = {{0, 1.5},{1, -1.5},{0, -1.5},{1, 1.5},{2,5.0}};

int gType;
int gIndex; 
float gPoint[3] = {0.0, 0.0, 0.0};


int main ()
{
        //float rand_ray [3];
	//int s_minus;
	//srand ((unsigned int)time(NULL));
	//for(int i=0; i<3; i++){	
	// 	s_minus= rand() % 2;
        //      cout<< s_minus<<"\n";
	//	if(s_minus==0)
	//	 rand_ray[i]=((float)rand()/(float)RAND_MAX);
	//	if(s_minus==1)
	//	 rand_ray[i]=-((float)rand()/(float)RAND_MAX);
	//}
	//float *ray=normalize3(rand_ray);
        //cout << ray[0]<<";"<<ray[1]<<";"<<ray[2]<<"\n";
        float ray[3];
        klee_make_symbolic_with_sort(&ray, sizeof(ray), "ray", 8, 32);
        raySphere(0, ray, gOrigin);
        //raytrace(ray,gOrigin);
        return 0;
}

void raytrace(float *ray, float *origin)
{

    for (int t = 0; t < nrTypes; t++)
    for (int i = 0; i < nrObjects[t]; i++)
      rayObject(t,i,ray,origin);
}

void rayObject(int type, int idx, float *r, float *o){
  if (type == 0) raySphere(idx,r,o);
}

void raySphere(int idx, float *r, float *o) //Ray-Sphere Intersection: r=Ray Direction, o=Ray Origin
{ 
  int sw=0;
  float spheres_index[3];
  for(int i=0;i<3;i++)
	  spheres_index[i]=spheres[idx][i];

  float *s = sub3(spheres_index,o);  //s=Sphere Center Translated into Coordinate Frame of Ray Origin
  float radius = spheres[idx][3];    //radius=Sphere Radius
  
  //Intersection of Sphere and Line     =       Quadratic Function of Distance
  float A = dot3(r,r);                       // Remember This From High School? :
  float B = -2.0 * dot3(s,r);                //    A x^2 +     B x +               C  = 0
  float C = dot3(s,s) - radius*radius;          // (r'r)x^2 - (2s'r)x + (s's - radius^2) = 0
  float D = B*B - 4*A*C;                     // Precompute Discriminant
  //cout<<D <<"\n";
  if (D > 0.0) {
      sw=1;
  }
   //if(sw==0)
     // cout<< "did not enter (D> 0.0) \n";
}

float *normalize3(float *v){        //Normalize 3-Vector
  float L = sqrt(dot3(v,v));
  float *norm_v=mul3c(v, 1.0/L);
  return norm_v;
}

float *sub3(float *a, float *b){   //Subtract 3-Vectors
  float *result;
  result=(float*) malloc (12);
  for(int i=0;i<3;i++)
    result[i]=a[i] - b[i];
  return result;
}


float *mul3c(float* a, float c){    //Multiply 3-Vector with Scalar
  	float *result;
	result=(float*) malloc (3);
	for(int i=0; i<3;i++)
		result[i]=a[i]*c;
  return result;
}

float dot3(float *a,float  *b){     //Dot Product 3-Vectors
  float result= a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
  return result;
}


