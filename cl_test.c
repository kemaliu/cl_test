// copyright (c) 2016 Wladimir J. van der Laan
// Distributed under the MIT software license.
// Based on the Apple OpenCL "Hello World" demo
// gcc hello.c -o hello -O2 /usr/lib/x86_64-linux-gnu/libOpenCL.so.1
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <CL/cl.h>
#include <sys/time.h>

#include <sys/time.h>
unsigned int time_us_get()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    unsigned int us = now.tv_sec*1000000 + now.tv_usec;
    return us;
}

unsigned int time_us_diff(unsigned int last)
{
    unsigned int now = time_us_get();
    return now - last;
}
////////////////////////////////////////////////////////////////////////////////

// Use a static data size for simplicity
//

////////////////////////////////////////////////////////////////////////////////

// Simple compute kernel which computes the square of an input array
//

////////////////////////////////////////////////////////////////////////////////
// Dump binary to disk


#include <stdarg.h>

void dump_binaries(cl_program program, const char *name)
{
    int err;
    size_t retsize;
    size_t progcount = 0;
    size_t *sizes = NULL;
    void *data = NULL;
    void **binaries = NULL;

    err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, 0, NULL, &retsize);
    if (err != CL_SUCCESS) {
        printf("error requesting program binary sizes\n");
        goto cleanup;
    }
    progcount = retsize / sizeof(size_t);
    sizes = calloc(progcount, sizeof(size_t));
    err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, progcount*sizeof(void*), sizes, &retsize);

    size_t total_size = 0;
    for (size_t bid=0; bid<progcount; ++bid) {
        total_size += sizes[bid];
    }
    printf("success: got back %i binaries, total size %i\n", (int)progcount, (int)total_size);

    data = malloc(total_size);
    binaries = calloc(progcount, sizeof(void*));
    void *ptr = data;
    for (size_t bid=0; bid<progcount; ++bid) {
        binaries[bid] = ptr;
        ptr += sizes[bid];
    }
    err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, progcount*sizeof(void*), binaries, &retsize);
    if (err != CL_SUCCESS) {
        printf("error: CL_PROGRAM_BINARIES error\n");
        goto cleanup;
    }
    if (retsize != progcount * sizeof(void*)) {
        printf("error: CL_PROGRAM_BINARIES size mismatch\n");
        goto cleanup;
    }
    for (size_t bid=0; bid<progcount; ++bid) {
        char filename[80];
        FILE *f;
        snprintf(filename, sizeof(filename), "%s%i.gallium_bin", name, (int)bid);
        f = fopen(filename, "wb");
        fwrite(binaries[bid], sizes[bid], 1, f);
        fclose(f);
        printf("binary %i: size %i dumped to %s\n", (int)bid, (int)sizes[bid], filename);
    }
cleanup:
    if (sizes)
        free(sizes);
    if (binaries)
        free(binaries);
    if (data)
        free(data);
}

int do_test(int mode, int floattype, int size, int repeat, int print)
{
    int err;                            // error code returned from api calls
    float * data;
    float * results;
    float * data_pos;
    int input_data_num, output_data_num;
    if (mode == 0 || mode == 2){
        data = malloc(sizeof(float)*size*floattype);
        results  = malloc(sizeof(float)*size*floattype);
        data_pos  = malloc(sizeof(float)*0x200);
        input_data_num = (size)*floattype;
        output_data_num = (size)*floattype;
    }else if (mode == 1){
        data = malloc(sizeof(float)*(size)*3);
        results  = malloc(sizeof(float)*(size));
        data_pos  = malloc(sizeof(float)*0x200);
        input_data_num = (size)*3;
        output_data_num = size;
    }else if (mode == 1){
        data = malloc(sizeof(float)*(size + 1024*1024)*3);
        results  = malloc(sizeof(float)*(size+1024));
        data_pos  = malloc(sizeof(float)*0x200);
        input_data_num = (size)*3;
        output_data_num = size;
    }else{
        printf("unknow mode %d\n", mode);
        return -1;
    }
    
    unsigned int correct;               // number of correct results returned

    size_t global;                      // global domain size for our calculation
    size_t local;                       // local domain size for our calculation

    cl_device_id device_id;             // compute device id
    cl_context context;                 // compute context
    cl_command_queue commands;          // compute command queue
    cl_program program;                 // compute program
    cl_kernel kernel;                   // compute kernel

    cl_mem input;                       // device memory used for the input array
    cl_mem output;                      // device memory used for the output array
    cl_mem pos;                      // device memory used for the output array
    
    // Fill our data set with random float values
    //
    int i = 0, j;
    unsigned int count = size;
    for(i = 0; i < input_data_num; i++){
#if 0
        data[i] = (float)rand() / (float)RAND_MAX;
#else
        data[i] = (float)1.0;
#endif
    }
    for(i = 0; i < 0x200; i++){
        data_pos[i] = (float)i;/*(float)rand() / (float)RAND_MAX;*/
    }
    cl_uint platformCount = 0;
    cl_platform_id* platforms;
    if (clGetPlatformIDs(0, NULL, &platformCount) != CL_SUCCESS) {
        printf("Unable to get platform IDs\n");
        exit(1);
    }
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    if (clGetPlatformIDs(platformCount, platforms, NULL) != CL_SUCCESS) {
        printf("Unable to get platform IDs\n");
        exit(1);
    }
    // Connect to a compute device
    //
    int gpu = 1;
    err = clGetDeviceIDs(platforms[0], gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to create a device group!\n");
        return EXIT_FAILURE;
    }

    size_t paramValueSize;
    err = clGetDeviceInfo(device_id, CL_DEVICE_EXTENSIONS, 0, NULL, &paramValueSize);
    if(err == CL_SUCCESS){
        char * info = malloc(paramValueSize);
        err = clGetDeviceInfo(device_id, CL_DEVICE_EXTENSIONS, paramValueSize, info , NULL);
        if(err == CL_SUCCESS){
            printf("ext::::\n");
            printf("%s\n", info);
        }else{
            printf("clGetDeviceInfo 2 failed!!!!\n");
        }
    }else{
        printf("clGetDeviceInfo failed!!!!\n");
        sleep(1);
    }
    // Create a compute context
    //
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if (!context)
    {
        printf("Error: Failed to create a compute context!\n");
        return EXIT_FAILURE;
    }

    // Create a command commands
    //
    commands = clCreateCommandQueue(context, device_id, 0, &err);
    if (!commands)
    {
        printf("Error: Failed to create a command commands!\n");
        return EXIT_FAILURE;
    }

    // Create the compute program from the source buffer
    //

    char * src_buf = malloc(0x10000);
    memset(src_buf, 0, 0x10000);
    int fd = -1;
    if(mode == 0){              /* copy */
        if(floattype==1){
            fd = open("copy_f1.cl", 2, 666);
        }else if(floattype==2){
            fd = open("copy_f2.cl", 2, 666);
        }else if(floattype==3){
            fd = open("copy_f3.cl", 2, 666);
        }else if(floattype==4){
            fd = open("copy_f3.cl", 2, 666);
        }
    }else if(mode == 1){              /* sqrt */
        if(floattype==1){
            fd = open("sqrt_f1.cl", 2, 666);
        }else if(floattype==3){
            fd = open("sqrt_f3.cl", 2, 666);
        }
    }
    read(fd, src_buf, 0x10000);
    close(fd);
    program = clCreateProgramWithSource(context, 1, (const char **) &src_buf, NULL, &err);
    if (!program)
    {
        printf("Error: Failed to create compute program!\n");
        return EXIT_FAILURE;
    }

    // Build the program executable
    //
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }
    dump_binaries(program, "square");

    // Create the compute kernel in the program we wish to run
    //
    kernel = clCreateKernel(program, "square", &err);
    if (!kernel || err != CL_SUCCESS)
    {
        printf("Error: Failed to create compute kernel!\n");
        exit(1);
    }

    // Create the input and output arrays in device memory for our calculation
    //

#if 0
    inputx = clCreateBuffer(context,  CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR,  sizeof(float) * input_data_num, data, NULL);
#else
    input = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * input_data_num, NULL, NULL);
#endif
    output = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * output_data_num, NULL, NULL);
    pos = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * (0x200), NULL, NULL);
    if (!input || !output || !pos)
    {
        printf("Error: Failed to allocate device memory!\n");
        exit(1);
    }

    // Write our data set into the input array in device memory
    //
    printf("clEnqueueWriteBuffer\n");
#if 1
    err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, sizeof(float) * input_data_num, data, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to write to source x array!\n");
        exit(1);
    }
#endif
    err = clEnqueueWriteBuffer(commands, pos, CL_TRUE, 0, sizeof(float) * (0x200), data_pos, 0, NULL, NULL);
    if (err != CL_SUCCESS){
        printf("Error: Failed to write to source pos array!\n");
        exit(1);
    }
        
    // Set the arguments to our compute kernel
    //
    err = 0;
    printf("clEnqueueWriteBuffer\n");
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
    err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &count);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &pos);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to set kernel arguments! %d\n", err);
        exit(1);
    }

    // Get the maximum work group size for executing the kernel on the device
    //
    err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        exit(1);
    }

    // Execute the kernel over the entire range of our 1d input data set
    // using the maximum number of work group items for this device
    //
    global = size;
    printf("group size %u, global size 0x%x\n", local, count);
    clFinish(commands);
    local = 256;
    unsigned int time = time_us_get();
    printf("global 0x%x\n", global);

    for(i=0; i<repeat; i++){
        err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
        /* clFinish(commands); */
    }

    if (err)
    {
        printf("Error: Failed to execute kernel!\n");
        return EXIT_FAILURE;
    }

    // Wait for the command commands to get serviced before reading back results
    //

    clFinish(commands);

    time = time_us_diff(time);
    // Read back the results from the device to verify the output
    //
    err = clEnqueueReadBuffer( commands, output, CL_TRUE, 0, sizeof(float) * size, results, 0, NULL, NULL );
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to read output array! %d\n", err);
        exit(1);
    }

    // Validate our results
    //
    correct = 0;
    for(i = 0; i < output_data_num; i++)
    {
        float check = 0.0;
        if(mode == 0){          /* copy */
            check += data[i];
        }else if(mode == 1){          /* sqrt */
#if 1
            for(j=0; j<32; j++)
                check += sqrt((data[i*3+0]-data_pos[j*3+0])*(data[i*3+0]-data_pos[j*3+0]) +
                              (data[i*3+1]-data_pos[j*3+1])*(data[i*3+1]-data_pos[j*3+1]) +
                              (data[i*3+2]-data_pos[j*3+2])*(data[i*3+2]-data_pos[j*3+2]));
#else
            check = data_pos[0];
#endif
        }else if(mode == 2){          /* mul */
            check += data[i] * data[i];
        }
        
        if(fabsf(results[i] - check) < 0.001){
            if((print&2)){       /* print correct */
                printf("ok %d %f %f diff %f\n", i, results[i], check, fabsf(results[i] - check));
            }
            correct++;
            /* printf("ok %d %f %f\n", i, results[i], check); */
        }else{
            if(print&1){       /* print correct */
                printf("err %d %f(%x) %f(%x), diff %f\n", i, results[i], *(unsigned int *)&results[i], check, *(unsigned int *)&check,fabsf(results[i] - check));
            }
        }
    }

    // Print a brief summary detailing the results
    //
    printf("Computed '%d/%d' correct values!\n", correct, output_data_num);
    
    printf("-------size %x(0x%x*%u*%d) cost %u us, speed %fM/s\n",
           size*repeat*floattype, size,repeat,floattype, time, 
           (float)output_data_num*repeat/1024.0/1024.0/((float)time/1000000.0));
    // Shutdown and cleanup
    //
    clReleaseMemObject(input);
    clReleaseMemObject(output);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
    return 0;
}

int main(int argc, char ** argv)
{
    int mode, floattype, size=0x100000, repeat=100, print=0;
    if(argc<3){
        printf("cl_test usage:\n");
        printf("cl_test mode floattype size repeat printmode\n");
        printf("mode copy: do copy test\n");
        printf("     sqrt: do sqrt test\n");
        printf("     mul: do mul test(output = input*input)\n");
        printf("type 1:float 2:float2 3:float3 4:float4\n");
        printf("size number of float\n");
        printf("repeat: repeat num\n");
        return -1;
    }
    if(strcmp(argv[1], "copy")==0){
        mode = 0;
    }else if(strcmp(argv[1], "sqrt")==0){
        mode = 1;
    }else if(strcmp(argv[1], "cf")==0){
        mode = 2;
    }else{
        printf("mode <%s> illegal\n", argv[1]);
        return -1;
    }


    if(strcmp(argv[2], "1")==0){
        floattype = 1;
    }else if(strcmp(argv[2], "2")==0){
        floattype = 2;
    }else if(strcmp(argv[2], "3")==0){
        floattype = 3;
    }else if(strcmp(argv[2], "4")==0){
        floattype = 4;
    }else{
        printf("floattype <%s> illegal\n", argv[2]);
        return -2;
    }
    if(argc >= 4)
        size = strtol(argv[3], NULL, 0);

    if(argc >= 5)
        repeat = strtol(argv[4], NULL, 0);
    if(argc >= 6)
        print = strtol(argv[5], NULL, 0);
    printf("mode %d type %d size 0x%x repeat %u print %d\n", mode, floattype, size, repeat, print);
    do_test(mode, floattype, size, repeat, print);
        
}
