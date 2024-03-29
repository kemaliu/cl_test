// Copyright (c) 2016 Wladimir J. van der Laan
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


////////////////////////////////////////////////////////////////////////////////

// Use a static data size for simplicity
//
#define DATA_SIZE (1024)

////////////////////////////////////////////////////////////////////////////////

// Simple compute kernel which computes the square of an input array
//
const char *KernelSource = "\n" \
"__kernel void square(                                                       \n" \
"   __global float* input,                                              \n" \
"   __global float* output,                                             \n" \
"   const unsigned int count)                                           \n" \
"{                                                                      \n" \
"   int i = get_global_id(0);                                           \n" \
"   if(i < count)                                                       \n" \
"       output[i] = input[i] * input[i];                                \n" \
"}                                                                      \n" \
"\n";

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

int main(int argc, char** argv)
{
    int err;                            // error code returned from api calls

    float data[DATA_SIZE];              // original data set given to device
    float results[DATA_SIZE];           // results returned from device
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

    // Fill our data set with random float values
    //
    int i = 0;
    unsigned int count = DATA_SIZE;
    kemaliu_memory_log("test my log %d\n", 0x1234);
    for(i = 0; i < count; i++)
        data[i] = rand() / (float)RAND_MAX;

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
#if 1
    {
#define BIN_SIZE 0x100000
        char * bin = malloc(BIN_SIZE);
        memset(bin, 0, BIN_SIZE);
        int fd = open("1.bin", 2, 666);
        size_t len = read(fd, bin, BIN_SIZE);
        cl_int errNum = 0;
        cl_int binaryStatus;
        program = clCreateProgramWithBinary(context, 1, &device_id, &len, &bin, &binaryStatus, &errNum);
        if(errNum != CL_SUCCESS){
            printf("clCreateProgramWithBinary failed\n");
            return EXIT_FAILURE;
        }else{
            printf("clCreateProgramWithBinary ok\n");
        }
    }
    
#else
    program = clCreateProgramWithSource(context, 1, (const char **) & KernelSource, NULL, &err);
#endif
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
    input = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * count, NULL, NULL);
    output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * count, NULL, NULL);
    if (!input || !output)
    {
        printf("Error: Failed to allocate device memory!\n");
        exit(1);
    }

    // Write our data set into the input array in device memory
    //
    printf("clEnqueueWriteBuffer\n");
    err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, sizeof(float) * count, data, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to write to source array!\n");
        exit(1);
    }

    // Set the arguments to our compute kernel
    //
    err = 0;
    printf("clEnqueueWriteBuffer\n");
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
    err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &count);
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
    global = count;
    kemaliu_memory_log("clEnqueueNDRangeKernel\n");
    kemaliu_set_log_level(3);
    err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
    kemaliu_set_log_level(0);
    kemaliu_memory_log("clEnqueueNDRangeKernel return %d\n", err);
    if (err)
    {
        printf("Error: Failed to execute kernel!\n");
        return EXIT_FAILURE;
    }

    // Wait for the command commands to get serviced before reading back results
    //
    kemaliu_memory_log("clFinish\n");
    clFinish(commands);
    kemaliu_memory_log("clFinish done\n");

    // Read back the results from the device to verify the output
    //
    err = clEnqueueReadBuffer( commands, output, CL_TRUE, 0, sizeof(float) * count, results, 0, NULL, NULL );
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to read output array! %d\n", err);
        exit(1);
    }

    // Validate our results
    //
    correct = 0;
    for(i = 0; i < count; i++)
    {
        if(results[i] == data[i] * data[i])
            correct++;
    }

    // Print a brief summary detailing the results
    //
    printf("Computed '%d/%d' correct values!\n", correct, count);

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

