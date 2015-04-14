#include <CL/cl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define X_SIZE 			(500)
#define Y_SIZE 			(500)

const char *ProgramSource =
"#define MAGNIFY 1.0\n"\
"#define X_SIZE 500\n"\
"#define Y_SIZE 500\n"\
"__kernel void hw_mandelbrot_frame(__global unsigned int* framebuffer)\n"\
"{\n"\
"	const size_t windowPosX = get_global_id(0);\n"\
"	const size_t windowPosY = get_global_id(1);\n"\
"       float x,xx,y,cx,cy;\n"\
"       int iteration;\n"\
"       cx = (((float)windowPosX+1)/((float)X_SIZE)-0.5)/MAGNIFY*3.0-0.7;\n"\
"       cy = (((float)windowPosY+1)/((float)Y_SIZE)-0.5)/MAGNIFY*3.0;\n"\
"       x = 0.0; y = 0.0;\n"\
"	for (iteration = 1; iteration < 100; iteration++)\n"\
"	{\n"\
"         	xx = x*x-y*y+cx;\n"\
"               y = 2.0*x*y+cy;\n"\
"               x = xx;\n"\
"		if (x*x+y*y>100.0) break;\n"\
"       }\n"\
"	framebuffer[X_SIZE * windowPosY + windowPosX] = iteration;\n"\
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
	cl_event event;

	cl_mem CLframebuffer;

	size_t global[2] = {X_SIZE, Y_SIZE};

	unsigned int* framebuffer = (float*) malloc (X_SIZE*Y_SIZE*sizeof(float));

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
	kernel = clCreateKernel(program, "hw_mandelbrot_frame", &err);


	CLframebuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int) * X_SIZE * Y_SIZE, NULL, NULL);

	clSetKernelArg(kernel, 0, sizeof(cl_mem), &CLframebuffer);

	// enqueue the kernel command for execution
	clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global, NULL, 0, NULL, &event);
	clWaitForEvents(1 , &event);
	clFinish(command_queue);
	cl_ulong time_start, time_end;
	double total_time;
 

	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time = time_end - time_start;

 
	clEnqueueReadBuffer(command_queue, CLframebuffer, CL_TRUE, 0, sizeof(unsigned int) * X_SIZE * Y_SIZE, framebuffer, 0, NULL, NULL);


	// print the results
	int i, j;
	for(j = 0; j < Y_SIZE; j++)
	{
		for(i = 0; i < X_SIZE; i++)
			printf("%X\n", framebuffer[X_SIZE * j + i]);
	}
 
	// cleanup - release OpenCL resources
	clReleaseMemObject(CLframebuffer);

	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);
 
	return 0;
}
