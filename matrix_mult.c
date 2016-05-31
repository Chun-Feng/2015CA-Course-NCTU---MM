#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "matrix_mult.bin"
#define KERNEL_FUNC "matrix_mult"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "parameter.h"

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif


cl_device_id create_device() {

    cl_platform_id platform;
    cl_device_id dev;
    int err;

/* Identify a platform */
    err = clGetPlatformIDs(1, &platform, NULL);
    if(err < 0) {
        perror("Couldn't identify a platform");
        exit(1);
    } 

/* Access a device */
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
    if(err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
    }
    if(err < 0) {
        perror("Couldn't access any devices");
        exit(1);   
    }

    return dev;
}


/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

	cl_program program;
	cl_int err;
    char *binary_buffer, *program_log;
    FILE *program_handle;
    size_t binary_size, log_size;
    cl_int binary_status;
    program_handle = fopen(PROGRAM_FILE, "r");
    if(program_handle ==NULL){
         perror("Couldn't find the program file.");
         exit(1);
    }

    binary_buffer = (char*)malloc(0x100000);
    binary_size = fread(binary_buffer, 1, 0x100000, program_handle);
    fclose(program_handle);

    // Create program from binary file
    program = clCreateProgramWithBinary(ctx, 1, &dev, (const size_t*)&binary_size, (const unsigned char**)&binary_buffer, &binary_status, &err);
    if(err < 0){
         printf("%d\n", err);
         perror("Couldn't create the Program.");
         exit(1);
    }
    free(binary_buffer);
    //Build program
    err = clBuildProgram(program, 1, &dev, NULL, NULL, NULL);
    if(err < 0){
         //Find the size of log and print to std output
         clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
         program_log = (char*) malloc(log_size +1);
         program_log[log_size] = '\0';
         clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
         printf("%s\n", program_log);
         free(program_log);
         exit(1);
    }

   return program;
}

void Generate_Matirx(int *A, int *B, int M, int N){
	srand(time(NULL));
	int i;
	for(i=0; i<M*N; i++){
		A[i] = (int)(10.0*rand()/(RAND_MAX+1.0));
		B[i] = (int)(10.0*rand()/(RAND_MAX+1.0));
	}
}
int main(){

   /*****************************************/
   /* OpenCL specific variables */
   /*****************************************/      
    cl_device_id dev;
    cl_context ctx;
    cl_command_queue queue;
    cl_program program;
    cl_kernel matrix_mult;
    cl_mem input_a, input_b, output_c;
    cl_int err, i, j, k;
	
    int Mdim = 256;
    int Ndim = 256;
    int size_a = Mdim * Ndim;	                     // A = M * N matrix
    int size_b = Ndim * Mdim;	                     // B = N * M matrix
    int size_c = Mdim * Mdim; 	                  // C = M * M matrix
    size_t global_size[] = {Mdim, Mdim};           // the total number of work-item is equal to num_element of C
 
    /*****************************************/
    /* Initialize Host memory */
    /*****************************************/   
	 int *A = malloc(Mdim*Ndim*sizeof(int));
	 int *B = malloc(Ndim*Mdim*sizeof(int));
	 int *C = malloc(Mdim*Mdim*sizeof(int));
	 int *check = malloc(Mdim*Mdim*sizeof(int));

    /* Initializing matrix A and B */
    Generate_Matirx(A,B,Mdim,Ndim);
/*	 for(i=0; i<Mdim; i++) {
		 for(j=0; j<Mdim; j++) {
			 for(k=0; k<Ndim; k++) {
				 check[i*Mdim+j] += A[i*Ndim+k] * B[k*Mdim+j];
			 }
		 }
	 }*/
    /*****************************************/
    /* Initialize OpenCL */
    /*****************************************/   
    /* Create a device and context */
    dev = create_device();
    ctx = clCreateContext(NULL, 1, &dev, NULL, NULL, &err);
    if(err < 0){
        perror("couldn't create a context");
        exit(1);
    };
    
    /* Build the program */
    program = build_program(ctx, dev, PROGRAM_FILE);

    /* Create a kernel for the multiplication function */
    matrix_mult = clCreateKernel(program, KERNEL_FUNC, &err);
    if(err < 0){
        perror("couldn't create a kernel");
        exit(1);
    };

    /* Create a command queue */
    queue = clCreateCommandQueue(ctx, dev, 0 , &err);
    if(err < 0) {
        perror("Couldn't create a command queue");
        exit(1);   
    };
    
    /* Create buffer */
    input_a = clCreateBuffer(ctx, CL_MEM_READ_ONLY |
            CL_MEM_COPY_HOST_PTR, size_a * sizeof(int), A, &err);
    input_b = clCreateBuffer(ctx, CL_MEM_READ_ONLY | 
            CL_MEM_COPY_HOST_PTR, size_b * sizeof(int), B, &err);
    output_c = clCreateBuffer(ctx, CL_MEM_READ_WRITE, 
            size_c * sizeof(int), NULL, &err);
    if(err < 0) {
        perror("Couldn't create the buffer");
        exit(1);   
    };

    /* Set arguments for vector kernel */
    err = clSetKernelArg(matrix_mult, 0, sizeof(int), &Mdim);
    err |= clSetKernelArg(matrix_mult, 1, sizeof(int), &Ndim);
    err |= clSetKernelArg(matrix_mult, 2, sizeof(cl_mem), &input_a);
    err |= clSetKernelArg(matrix_mult, 3, sizeof(cl_mem), &input_b);
    err |= clSetKernelArg(matrix_mult, 4, sizeof(cl_mem), &output_c);
    if(err < 0) {
        perror("Couldn't create a kernel argument");
        exit(1);
    }

    /* Enqueue kernels */    
    err = clEnqueueNDRangeKernel(queue, matrix_mult, 2, NULL, global_size,
        NULL , 0, NULL, NULL);
    if(err < 0) {
        perror("Couldn't enqueue the kernel");
        exit(1);
    }
    err = clEnqueueReadBuffer(queue, output_c, CL_TRUE, 0,
            size_c * sizeof(int), C, 0, NULL, NULL);
    if(err < 0) {
        perror("Couldn't enqueue read buffer");
        exit(1);
    }			

    /* Profile time test */ 
	 // Step1: cl_event ...
	 // Step2: Please go back to set the third argument of clCreateCommandQueue to "CL_QUEUE_PROFILING_ENABLE"
	 // Step3: Associate with "Command" such as clEnqueueNDRangeKernel and clEnqueueReadBuffer ... 
	 // Step4: clGetEventProfilingInfo(...)

    
	 /*****************************************/   
    /* Check the result with CPU */
    /*****************************************/   
    err = 0;
    for (i=0; i<Mdim; i++){
        for (j=0; j<Mdim; j++){
            if (C[i*Mdim+j] != check[i*Mdim+j]){
                err = -1;
               // printf("error occur in (%d, %d)\n", i, j);
            }
        }
    }
    if (err == 0){
        printf("Check pass\n");
    }
    else {
        printf("Check fail\n");
    }
	
    /*****************************************/
    /* Deallocate resources */
    /*****************************************/
    clReleaseMemObject(input_a);
    clReleaseMemObject(input_b);
    clReleaseMemObject(output_c);
    clReleaseKernel(matrix_mult);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(ctx);
    /* Deallocate event */

    return 0;
}
