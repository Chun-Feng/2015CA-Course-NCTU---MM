#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <CL/cl.h>
//#include "parameter.h"

// ---------- Function for OpenCL structure  
// Create a device structure
cl_device_id create_device() {

    cl_platform_id platform;
    cl_device_id dev;
    int err;

    // Identify a platform
    err = clGetPlatformIDs(1, &platform, NULL);
    if(err < 0) {
        printf("Couldn't identify a platform: %d\n", err);
        exit(1);
    } 

    // Access a device
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
    if(err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
    }
    if(err < 0) {
        printf("Couldn't access any devices: %d\n", err);
        exit(1);
    }
	
    return dev;
}

cl_context create_context(cl_device_id dev) {

	cl_context ctx;
	int err;
	ctx = clCreateContext(NULL, 1, &dev, NULL, NULL, &err);
    if(err < 0){
        printf("couldn't create a context: %d\n", err);
        exit(1);
    };
	
	return ctx;
	
}

cl_kernel create_kernel(cl_program prog){
    cl_kernel kn;
    int err;
    kn = clCreateKernel(prog, "matrix_mult", &err);
    if(err < 0){
        printf("Couldn't create a kernel: %d\n", err);
        exit(1);
    };
	return kn;
}

cl_command_queue create_queue(cl_device_id dev, cl_context ctx){
    cl_command_queue queue;
    int err;
    queue = clCreateCommandQueue(ctx, dev, 0, &err);
    if(err < 0) {
        printf("Couldn't create a command queue: %d\n", err);
        exit(1);
    };
	return queue;
}


// Create program from a file and compile it 
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

    cl_program program;
    cl_int err;
    char *binary_buffer, *program_log;
    FILE *program_handle;
    size_t binary_size, log_size;
    cl_int binary_status;
    program_handle = fopen(PROGRAM_FILE, "r");
    if(program_handle ==NULL){
         printf("Couldn't find the program file.");
         exit(1);
    }

    binary_buffer = (char*)malloc(0x100000);
    binary_size = fread(binary_buffer, 1, 0x100000, program_handle);
    fclose(program_handle);

    // Create program from binary file
    program = clCreateProgramWithBinary(ctx, 1, &dev, (const size_t*)&binary_size, (const unsigned char**)&binary_buffer, &binary_status, &err);
    if(err < 0){
         printf("Couldn't create the Program: %d\n", err);
         exit(1);
    }
    free(binary_buffer);
	
    // Build program
    err = clBuildProgram(program, 1, &dev, NULL, NULL, NULL);
    if(err < 0){
         // Find the size of log and print to std output
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
// ---------- Function for host
  	
void Matrix_Mult_CPU(int *MatA, int *MatB, int *MatC){
	int i, j, k;
    for(i=0; i<HeightA; i++) {
        for(j=0; j<WidthB; j++) {
            for(k=0; k<WidthA; k++) {
                MatC[i*WidthB+j] += MatA[i*WidthA+k] * MatB[k*WidthB+j];
            }
        }
    }	
}

void Generate_Matirx(int *A, int *B, int M, int N, int P){
    srand(time(NULL));
    int i;
	
    for(i=0; i<M*N; i++){
        A[i] = (int)(10.0*rand()/(RAND_MAX+1.0));
    }
    for(i=0; i<N*P; i++){
        B[i] = (int)(10.0*rand()/(RAND_MAX+1.0));
    }
}

int Verify(int *C, int *CHECK){
    int i, j;
    for (i=0; i<HeightA; i++){
        for (j=0; j<WidthB; j++){
            if (C[i*WidthB+j]!=CHECK[i*WidthB+j])
                return -1;
        }
    }
	return 0;
}

int main(){
    /*****************************************/
    /* Set up host memory */
    /*****************************************/
    int hA = HeightA;
    int wA = WidthA;
    int hB = HeightB;
    int wB = WidthB;
    int size_a = hA * wA;                // A = M * N matrix
    int size_b = hB * wB;                // B = N * M matrix
    int size_c = hA * wB;                // C = M * M matrix
    int err, i, j, k;	
	
    if(wA!=hB) {
        printf("Matrix size doesn't match!\n");
            exit(1);
    }

    int *A = malloc(hA*wA*sizeof(int));
    int *B = malloc(hB*wB*sizeof(int));
    int *gpuResult = malloc(hA*wB*sizeof(int));
    int *cpuResult = malloc(hA*wB*sizeof(int));

    /* Initialize Host memory */
    Generate_Matirx(A, B, hA, wA, wB);
	   for(i=0;i<hA;i++){
         for(j=0;j<wA;j++){
            for(k=0;k<wB;k++){
               cpuResult[i*hA+j] += A[i*wA+k] * B[k*wB+j];
            }
         }
      }

// CPU computation
// You could use this function to compute result by CPU

//     Matrix_Mult_CPU(A, B, cpuResult);
   
	
    /*****************************************/
    /* Initialize OpenCL */
    /*****************************************/  
    cl_device_id dev;
    cl_context ctx;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem d_a, d_b, d_c;
	
    dev = create_device();
    ctx = create_context(dev);
    program = build_program(ctx, dev, PROGRAM_FILE);
    kernel = create_kernel(program);
    queue = create_queue(dev, ctx);
    
    // Create buffer, send arguments, enqueue command ...  
    
    // Write your code here --------------------------------
    /* Setup device memory buffer */
    d_a = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float)*size_a, A, &err);
    d_b = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float)*size_b, B, &err);
    d_c = clCreateBuffer(ctx, CL_MEM_READ_ONLY , sizeof(cl_float)*size_c, NULL, &err);

    if(err<0){
      printf("couldn't create a buffer: %d\n", err);
      exit(1);
    };

    /* Create a kernel for the multiplication function */
    kernel = clCreateKernel(program, kernel, &err);
    if(err<0){
      printf("couldn't create a kernel: %d\n", err);
      exit(1);
    };
   
    /* Create a command quene */
    queue = clCreateCommandQueue(ctx, dev, 0, err);
    if(err<0){
      printf("couldn't create a command queue: %d\n", err);
      exit(1);
    };

    /* Create kernel argument */
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&d_a);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&d_b);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&d_c);

    if(err<0){
      printf("couldn't create a kernel argument: %d\n", err);
      exit(1);
    };

    /* Enquene kernels */
    size_t work_size = size_c;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &work_size, 0, 0, 0, 0);
    if(err<0){
      printf("couldn't enqueue the kernel: %d\n", err);
      exit(1);
    };

    err = clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0, size_c*sizeof(float), cpuResult, 0, NULL, NULL);
    if(err<0){
      printf("couldn't read buffer: %d\n", err);
      exit(1);
    };
	
    /*****************************************/   
    // Compare the result with CPU */
    // You could use this part of code to compare GPU's result with CPU
    /*****************************************/   
 	
    err = 1;
    for(i=0; i<hA;i++){
      if(cpuResult[i] != gpuResult[i]){
         err=0;
      }
    }
	
    if (err == 1){
        printf("Matrix size: %d x %d\n", hA, wB);
        printf("Check pass\n");
    }
    else {
        printf("Matrix size: %d x %d\n", hA, wB);
        printf("Check fail\n");
    }

    /*****************************************/
    /* Deallocate resources */
    /*****************************************/
    free(d_a);
    free(d_b);
    free(d_c);
    
    clReleaseContext(ctx);
    clReleaseCommandQueue(queue);
    clReleaseKernel(kernel);
    clReleaseProgram(program);

    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_c);

    return 0;
}
