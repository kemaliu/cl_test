__kernel void square(                                                        
   __global float3* input,
   __global float3* output,
   const unsigned int count,
   __global float* pos)
{
    int i = get_global_id(0);
    output[i] = input[i];
}                                                                       
