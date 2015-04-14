#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CL/cl.h>

#define DATA_SIZE 408

const char *ProgramSource =
"__kernel void RayTracing(__global float *ray, __global float *sphere, __global float *SphereRadius, __global float *Dis1,__global float *Dis2)\n"\
"{\n"\
"size_t id = get_global_id(0);\n"\
"float r[3]={ray[id*3],ray[id*3+1],ray[id*3+2]};\n"\
"float s[3]={sphere[id*3],sphere[id*3+1],sphere[id*3+2]};\n"\
"float A = 0;\n"\
"A = A + r[0]*r[0];\n"\
"A = A + r[1]*r[1];\n"\
"A = A + r[2]*r[2];\n"\
"float B1 = 0;\n"\
"B1 = B1 + s[0]*r[0];\n"\
"B1 = B1 + s[1]*r[1];\n"\
"B1 = B1 + s[2]*r[2];\n"\
"float B = -2.0*B1;\n"\
"float C1 = 0;\n"\
"C1 = C1 + s[0]*s[0];\n"\
"C1 = C1 + s[1]*s[1];\n"\
"C1 = C1 + s[2]*s[2];\n"\
"float C = C1 - SphereRadius[id];\n"\
"float D = B*B - 4.*(A*C);\n"\
"Dis1[id] = (D > 0) ? 1 : -1;\n"\
"Dis2[id] = D;\n"\
"}\n";

int main()
{
cl_context context;
cl_context_properties properties[3];
cl_kernel kernel;
cl_command_queue command_queue;
cl_program program;
cl_int err;
cl_uint num_of_platforms=0;
cl_platform_id platform_id;
cl_device_id device_id;
cl_uint num_of_devices=0;
cl_mem CLRay,CLSphere,CLSphereRadius,CLDis1,CLDis2;
cl_event event;
 
size_t global,local;


float *Ray=(float*) malloc (DATA_SIZE*3*sizeof(float));
float *Sphere=(float*) malloc (DATA_SIZE*3*sizeof(float));
float *SphereRadius=(float*) malloc (DATA_SIZE*sizeof(float));
float *Dis1=(float*) malloc (DATA_SIZE*sizeof(float));
float *Dis2=(float*) malloc (DATA_SIZE*sizeof(float));

int i,rsign,m=2000;
time_t second; 
srand( time(&second)); 
	
//sx sy sz rx ry rz radius
FILE* fp = fopen("inputs", "r");
if (fp == NULL) 
{
  	fprintf(stderr, "Can't open inputs file!\n");
  	exit(1);
}

int k;
for (k = 0; k < DATA_SIZE; k++)
{
	fscanf(fp, "%f %f %f %f %f %f %f",
	&Ray[k*3],
	&Ray[k*3+1],
	&Ray[k*3+2],
	&Sphere[k*3],
	&Sphere[k*3+1],
	&Sphere[k*3+2],      
	&SphereRadius[k]);
}

fclose(fp);

// retreive a list of platforms avaible
if (clGetPlatformIDs(1, &platform_id, &num_of_platforms)!= CL_SUCCESS)
{
printf("Unable to get platform_id\n");
return 1;
}
 
// try to get a supported GPU device
if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_of_devices) != CL_SUCCESS)
{
printf("Unable to get device_id\n");
return 1;
}
 
// context properties list - must be terminated with 0
properties[0]= CL_CONTEXT_PLATFORM;
properties[1]= (cl_context_properties) platform_id;
properties[2]= 0;


 
// create a context with the GPU device
context = clCreateContext(properties,1,&device_id,NULL,NULL,&err);
 
// create command queue using the context and device
command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
 
// create a program from the kernel source code
program = clCreateProgramWithSource(context,1,(const char **) &ProgramSource, NULL, &err);


// compile the program
if (clBuildProgram(program, 0, NULL, NULL, NULL, NULL) == CL_BUILD_PROGRAM_FAILURE)
{
printf("Error building program\n");
 size_t log_size;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

    // Allocate memory for the log
    char *log = (char *) malloc(log_size);

    // Get the log
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

    // Print the log
    printf("%s\n", log);

return 1;
}
    
 
// specify which kernel from the program to execute
kernel = clCreateKernel(program, "RayTracing", &err);
 
// create buffers for the input and ouput

CLRay = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * DATA_SIZE*3, NULL, NULL);
CLSphere = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) *DATA_SIZE*3, NULL, NULL);
CLSphereRadius = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) *DATA_SIZE, NULL, NULL);
CLDis1 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) *DATA_SIZE, NULL, NULL);
CLDis2 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) *DATA_SIZE, NULL, NULL);

// load data into the input buffer
clEnqueueWriteBuffer(command_queue, CLRay, CL_TRUE, 0, sizeof(float) * DATA_SIZE*3, Ray, 0, NULL, NULL);
clEnqueueWriteBuffer(command_queue, CLSphere, CL_TRUE, 0, sizeof(float) * DATA_SIZE*3,Sphere, 0, NULL, NULL);
clEnqueueWriteBuffer(command_queue, CLSphereRadius, CL_TRUE, 0, sizeof(float) * DATA_SIZE,SphereRadius, 0, NULL, NULL);

// set the argument list for the kernel command
clSetKernelArg(kernel, 0, sizeof(cl_mem), &CLRay);
clSetKernelArg(kernel, 1, sizeof(cl_mem), &CLSphere);
clSetKernelArg(kernel, 2, sizeof(cl_mem), &CLSphereRadius);
clSetKernelArg(kernel, 3, sizeof(cl_mem), &CLDis1);
clSetKernelArg(kernel, 4, sizeof(cl_mem), &CLDis2);

global=DATA_SIZE;
 
// enqueue the kernel command for execution
clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global, NULL, 0, NULL, &event);
clWaitForEvents(1 , &event);
clFinish(command_queue);
cl_ulong time_start, time_end;
double total_time;


clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
total_time = time_end - time_start;
//printf("\nExecution time in milliseconds = %0.3f ms\n", (total_time / 1000000.0) );

 
// copy the results from out of the output buffer
clEnqueueReadBuffer(command_queue, CLDis1, CL_TRUE, 0, sizeof(float)*DATA_SIZE,Dis1 , 0, NULL, NULL);
clEnqueueReadBuffer(command_queue,CLDis2 , CL_TRUE, 0, sizeof(float)*DATA_SIZE,Dis2 , 0, NULL, NULL);


/* Print the results */
int j; 
for(j=0;j<DATA_SIZE;j++)
        printf("%.18f\n", Dis2[j]);
printf("\n");

// cleanup - release OpenCL resources
clReleaseMemObject(CLRay);
clReleaseMemObject(CLSphere);
clReleaseMemObject(CLDis1);

clReleaseProgram(program);
clReleaseKernel(kernel);
clReleaseCommandQueue(command_queue);
clReleaseContext(context);
 
return 0;
 
}
