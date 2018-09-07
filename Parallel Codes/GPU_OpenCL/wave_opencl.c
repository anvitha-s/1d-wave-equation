#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define START_POINT 0
#define END_POINT 1 

#define MAX_SOURCE_SIZE (0x100000)

int main(void) {
    // Create the two input vectors
    int i;
    const int n = 10000;
    const int LIST_SIZE = n+1;
    int no_proc = 1000;
    double a_0 = START_POINT, b_n = END_POINT;
    double  h = (b_n - a_0)/n;
    double k = h;
    double *u_old = (double*)malloc(LIST_SIZE*sizeof(double));
    double *u_old2 = (double*)malloc(LIST_SIZE*sizeof(double));
    double *params = (double*)malloc(3*sizeof(double)); 

    params[0] = k;
    params[1] = h;
    params[2] = (double)(n);

    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("wave_kernel.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }

    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );
    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, 
            &device_id, &ret_num_devices);
   
    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device for each vector 
    cl_mem params_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
            sizeof(double)*3, NULL, &ret);
    cl_mem uOld2_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
            sizeof(double)*LIST_SIZE, NULL, &ret);
    cl_mem uOld_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
            sizeof(double)*LIST_SIZE, NULL, &ret);
    cl_mem u_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
            sizeof(double)*n, NULL, &ret);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, 
            (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (ret != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "wave", &ret);

    // Setting Initial conditions
    
    for(int i=0; i<=n; i++)
    {
        u_old2[i] = pow(((double)(i*h)),2);	        		//u(x,0) = x^2
        u_old[i] = u_old2[i];					//u_t (x,0) = 0
    }

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&params_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&uOld2_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&uOld_mem_obj);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&u_mem_obj);

    ret = clEnqueueWriteBuffer(command_queue, params_mem_obj, CL_TRUE, 0,
            sizeof(double)*3, params, 0, NULL, NULL);
    double *u = (double*)malloc(sizeof(double)*n);
    for(int t = 1; t <=n;t++)
    {
        // Copy the arrays u_old2 and u_old and B to their respective memory buffers
        ret = clEnqueueWriteBuffer(command_queue, uOld2_mem_obj, CL_TRUE, 0,
                sizeof(double)*LIST_SIZE, u_old2, 0, NULL, NULL);
        ret = clEnqueueWriteBuffer(command_queue, uOld_mem_obj, CL_TRUE, 0, 
                sizeof(double)*LIST_SIZE, u_old, 0, NULL, NULL);
        
        // Execute the OpenCL kernel on the list
        size_t global_item_size = n; // Process the entire lists
        size_t local_item_size = global_item_size/no_proc; // Divide work items into groups of 64
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
                &global_item_size, &local_item_size, 0, NULL, NULL);

        // Read the memory buffer C on the device to the local variable C
        ret = clEnqueueReadBuffer(command_queue, u_mem_obj, CL_TRUE, 0, 
                n * sizeof(double), u, 0, NULL, NULL);
        memcpy(u_old2,u_old,LIST_SIZE*sizeof(double));
        memcpy(u_old,u,n*sizeof(double));
        // Update boundary conditions
        u_old[0] = pow(((double)(t*k)),2);				//u(0,t) = t^2
        u_old[n] = 1 + pow(((double)(t*k)),2);				//u(1,t) = 1 + t^2
    }

    // Display the result to the screen
    
      for(int i=0; i<=10; i++)
      {
          printf("%f\t",u_old[i*(n/10)]);
      }

        /**
         * Displaying analytical solutions 
         */
      for(int i=0; i<=10; i++)
      {
        printf("\n");
        for(int j=0; j<=10; j++)
        {
          printf("%f\t",pow((double)(i*h*n)/10,2)+pow((double)(j*h*n)/10,2));
        }
        printf("\n");
      }
    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(uOld2_mem_obj);
    ret = clReleaseMemObject(uOld_mem_obj);
    ret = clReleaseMemObject(u_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(u_old2);
    free(u_old);
    free(u);
    return 0;
}
