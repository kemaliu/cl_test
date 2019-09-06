#define CC_NUM 8
__kernel void square(                                                        
   __global float* input_x,                                               
   __global float* input_y,                                               
   __global float* input_z,                                               
   __global float* output,                                              
   const unsigned int count)                                            
{                                                                       
   int i = get_global_id(0);                                            
   if(i < count){
       int start = i*CC_NUM;
       
#if 0
#if 0
       output[start] = get_local_id(0);
#else
       output[start] = sqrt(input_x[start]*input_x[start] + input_y[start]*input_y[start]+ input_z[start]*input_z[start]);       

       output[start+1] = sqrt(input_x[start+1]*input_x[start+1] + input_y[start+1]*input_y[start+1]+ input_z[start+1]*input_z[start+1]);
       output[start+2] = sqrt(input_x[start+2]*input_x[start+2] + input_y[start+2]*input_y[start+2]+ input_z[start+2]*input_z[start+2]);
       output[start+3] = sqrt(input_x[start+3]*input_x[start+3] + input_y[start+3]*input_y[start+3]+ input_z[start+3]*input_z[start+3]);
       output[start+4] = sqrt(input_x[start+4]*input_x[start+4] + input_y[start+4]*input_y[start+4]+ input_z[start+4]*input_z[start+4]);
       output[start+5] = sqrt(input_x[start+5]*input_x[start+5] + input_y[start+5]*input_y[start+5]+ input_z[start+5]*input_z[start+5]);
       output[start+6] = sqrt(input_x[start+6]*input_x[start+6] + input_y[start+6]*input_y[start+6]+ input_z[start+6]*input_z[start+6]);
       output[start+7] = sqrt(input_x[start+7]*input_x[start+7] + input_y[start+7]*input_y[start+7]+ input_z[start+7]*input_z[start+7]);
#endif
#else
       int j =0;
       for(j=0;j<CC_NUM;j++){
#if 1
           output[start+j] = (sqrt(input_x[start+j]*input_x[start+j] + input_y[start+j]*input_y[start+j] + input_z[start+j]*input_z[start+j]));
#else
           output[start+j] = input_x[start+j] + input_y[start+j]  + input_z[start+j];
#endif
           
       }
#endif
   }                                                                     
}                                                                       
