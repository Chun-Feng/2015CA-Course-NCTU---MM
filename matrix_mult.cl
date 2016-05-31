#include "parameter.h"

__kernel void matrix_mult(__const int Mdim, __const int Ndim,
        __global int *A, __global int *B , __global int *C){

    
    int k;
    int gx = get_global_id(0);
    int gy = get_global_id(1);
    
    int temp = 0;
    for(k=0; k<Ndim; k++){
        temp += A[gy*Ndim+k]*B[k*Mdim+gx];
    }
    C[gy*Mdim+gx] = temp;  

}
