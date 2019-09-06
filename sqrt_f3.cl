__kernel void square(                                                        
   __global float* input,                                               
   __global float* output,
   const unsigned int count,
   __global float* pos)                                            
{                                                                       
    int i = get_global_id(0);
    int j;
    float val = 0.0f;
    int idA=3*i;
    float3 A={input[idA], input[idA+1], input[idA+2]};
    
    for(j=0; j<96; j+=3){
        float3 B={pos[j], pos[j+1], pos[j+2]};
#if 0
        val+=distance(A, B);
#else
        float3 x = A-B;
        val += sqrt(dot(x, x));
#endif
    }
    output[i] = val;
}
