__kernel void wave(__global const double *params, __global const double *u_old2, __global const double *u_old, __global double *u) {
    // Get the index of the current element to be processed
    int i = get_global_id(0);

    // Do the operation
    if(i > 0)
        u[i] = 2*u_old[i] - u_old2[i] + ((params[0]*params[0])/(params[1]*params[1]))*(u_old[i+1] - 2*u_old[i] + u_old[i-1]);
}
