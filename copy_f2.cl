__kernel void square(                                                        
   __global float2* input,
   __global float2* output,
   const unsigned int count,
   __global float* pos)
{
    int i = get_global_id(0);
    output[i] = input[i];
}                                                                       
