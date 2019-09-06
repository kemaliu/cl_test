__kernel void square(                                                        
   __global float* input_x,                                               
   __global float* input_y,                                               
   __global float* input_z,                                               
   __global float* output,
   const unsigned int count,
   __global float* pos)                                            
{                                                                       
    int i = get_global_id(0);
    float val = 0.0;
    /*if(i < count)*/
    {
#if 1
        int k;
        for(k=0; k<3*8; k+=3){
            val += sqrt((input_x[i]-pos[k])*(input_x[i]-pos[k])+
                        (input_y[i]-pos[k+1])*(input_y[i]-pos[k+1])+
                        (input_z[i]-pos[k+2])*(input_z[i]-pos[k+2]));
        }
#else
        float x = input_x[i];
        float y = input_y[i];
        float z = input_z[i];
        val = sqrt((x-pos[0])*(x-pos[0])+
                  (y-pos[1])*(y-pos[1])+
                  (z-pos[2])*(z-pos[2]));

       val += sqrt((x-pos[3])*(x-pos[3])+
                   (y-pos[4])*(y-pos[4])+
                   (z-pos[5])*(z-pos[5]));
       val += sqrt((x-pos[6])*(x-pos[6])+
                   (y-pos[7])*(y-pos[7])+
                   (z-pos[8])*(z-pos[8]));
       
       val += sqrt((x-pos[9])*(x-pos[9])+
                   (y-pos[10])*(y-pos[10])+
                   (z-pos[11])*(z-pos[11]));
       
       val += sqrt((x-pos[12])*(x-pos[12])+
                   (y-pos[13])*(y-pos[13])+
                   (z-pos[14])*(z-pos[14]));
       val += sqrt((x-pos[15])*(x-pos[15])+
                   (y-pos[16])*(y-pos[16])+
                   (z-pos[17])*(z-pos[17]));
       val += sqrt((x-pos[18])*(x-pos[18])+
                   (y-pos[19])*(y-pos[19])+
                   (z-pos[20])*(z-pos[20]));
       val += sqrt((x-pos[21])*(x-pos[21])+
                   (y-pos[22])*(y-pos[22])+
                   (z-pos[23])*(z-pos[23]));
           /* 8 */
       val += sqrt((x-pos[24])*(x-pos[24])+
                   (y-pos[25])*(y-pos[25])+
                   (z-pos[26])*(z-pos[26]));
       val += sqrt((x-pos[27])*(x-pos[27])+
                   (y-pos[28])*(y-pos[28])+
                   (z-pos[29])*(z-pos[29]));
       val += sqrt((x-pos[30])*(x-pos[30])+
                   (y-pos[31])*(y-pos[31])+
                   (z-pos[32])*(z-pos[32]));
       val += sqrt((x-pos[33])*(x-pos[33])+
                   (y-pos[34])*(y-pos[34])+
                   (z-pos[35])*(z-pos[35]));

       val += sqrt((x-pos[36])*(x-pos[36])+
                   (y-pos[37])*(y-pos[37])+
                   (z-pos[38])*(z-pos[38]));
       val += sqrt((x-pos[39])*(x-pos[39])+
                   (y-pos[40])*(y-pos[40])+
                   (z-pos[41])*(z-pos[41]));
       val += sqrt((x-pos[42])*(x-pos[42])+
                   (y-pos[43])*(y-pos[43])+
                   (z-pos[44])*(z-pos[44]));
       val += sqrt((x-pos[45])*(x-pos[45])+
                   (y-pos[46])*(y-pos[46])+
                   (z-pos[47])*(z-pos[47]));
           /* 16 */
#if 0
       val += sqrt((x-pos[3])*(x-pos[3])+
                   (y-pos[4])*(y-pos[4])+
                   (z-pos[5])*(z-pos[5]));
       val += sqrt((x-pos[6])*(x-pos[6])+
                   (y-pos[7])*(y-pos[7])+
                   (z-pos[8])*(z-pos[8]));
       
       val += sqrt((x-pos[9])*(x-pos[9])+
                   (y-pos[10])*(y-pos[10])+
                   (z-pos[11])*(z-pos[11]));
       
       val += sqrt((x-pos[12])*(x-pos[12])+
                   (y-pos[13])*(y-pos[13])+
                   (z-pos[14])*(z-pos[14]));
       val += sqrt((x-pos[15])*(x-pos[15])+
                   (y-pos[16])*(y-pos[16])+
                   (z-pos[17])*(z-pos[17]));
       val += sqrt((x-pos[18])*(x-pos[18])+
                   (y-pos[19])*(y-pos[19])+
                   (z-pos[20])*(z-pos[20]));
       val += sqrt((x-pos[21])*(x-pos[21])+
                   (y-pos[22])*(y-pos[22])+
                   (z-pos[23])*(z-pos[23]));
           /* 24 */
       val += sqrt((x-pos[24])*(x-pos[24])+
                   (y-pos[25])*(y-pos[25])+
                   (z-pos[26])*(z-pos[26]));
       val += sqrt((x-pos[27])*(x-pos[27])+
                   (y-pos[28])*(y-pos[28])+
                   (z-pos[29])*(z-pos[29]));
       val += sqrt((x-pos[30])*(x-pos[30])+
                   (y-pos[31])*(y-pos[31])+
                   (z-pos[32])*(z-pos[32]));
       val += sqrt((x-pos[33])*(x-pos[33])+
                   (y-pos[34])*(y-pos[34])+
                   (z-pos[35])*(z-pos[35]));

       val += sqrt((x-pos[36])*(x-pos[36])+
                   (y-pos[37])*(y-pos[37])+
                   (z-pos[38])*(z-pos[38]));
       val += sqrt((x-pos[39])*(x-pos[39])+
                   (y-pos[40])*(y-pos[40])+
                   (z-pos[41])*(z-pos[41]));
       val += sqrt((x-pos[42])*(x-pos[42])+
                   (y-pos[43])*(y-pos[43])+
                   (z-pos[44])*(z-pos[44]));
       val += sqrt((x-pos[45])*(x-pos[45])+
                   (y-pos[46])*(y-pos[46])+
                   (z-pos[47])*(z-pos[47]));
       
       
#endif
#endif
	output[i] = val;
   }
}                                                                       
