__kernel void square(                                                        
   __global float* input,                                               
   __global float* output,
   const unsigned int count,
   __global float* pos)                                            
{                                                                       
    int i = get_global_id(0);
    int j;
    float val = 0.0f;
    for(j=0; j<32; j++){
        __private float x = (input[i*3] - pos[j*3+0]);
        __private float y = (input[i*3+1] - pos[j*3+1]);
        __private float z = (input[i*3+2] - pos[j*3+2]);
#if 1	  
        val += sqrt(x*x + y*y + z*z);
#else
        val = pos[0];
#endif
    }
    output[i] = val;
}
