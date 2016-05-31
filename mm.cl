//#include "parameter.h" 

__kernel void matrix_mult(	__const int Mdim, 
				__const int Ndim, 
				__const int Pdim, 
				__global int *A, 
				__global int *B, 
				__global int *C
			){							
// Write your code here
	
   int k;
   int i = get_global_id(0);
   int j = get_global_id(1);
   int tmp = 0;

   for(k=0;k<Pdim;k++)
      tmp += A[i*Ndim+k] * B[k*Pdim+j];
   C[i*Ndim+j] += tmp; 

}
