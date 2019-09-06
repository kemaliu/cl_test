__kernel void square(                                                        
   __global float4* input,
   __global float4* output,
   const unsigned int count,
   __global float* pos)
{
    int i = get_global_id(0);
    output[i] = input[i];
}                                                                       
