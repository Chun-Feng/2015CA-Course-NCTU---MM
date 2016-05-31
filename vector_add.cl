__kernel void vector_add(__global float *a, 
                         __global float *b, 
                        __global float *result)
{
	int idx = get_global_id(0);
	result[idx] = a[idx] + b[idx];
}


