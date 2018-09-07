#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NO_OF_POINTS 1000
#define pi 3.14159265359

void main(int argc, char** argv)
{
    int rank, no_proc,n = NO_OF_POINTS;
    double h=1.0/n, k=1.0/n, c=1;
    int tag = 0;

    MPI_Status stat;
    MPI_Init(&argc, &argv);
    
    // finding rank of process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // finding number of processors
    MPI_Comm_size(MPI_COMM_WORLD, &no_proc);

    int n_per_proc = n/no_proc;
    
    int num = n_per_proc+1;
    if(rank == no_proc - 1)
        num = n - n_per_proc*(no_proc-1) + 1;

    printf("rank : %d\tn_per_proc : %d\n",rank, n_per_proc);
    double *u = (double*)malloc(num*sizeof(double));
    double *u_old = (double*)malloc(num*sizeof(double));
    double *u_old2 = (double*)malloc(num*sizeof(double));

    //Setting initial conditions
    for(int i=0; i<num; i++)
    {
        u_old2[i] = 2;        //u(x,0) = 2
        u_old[i] = u_old2[i] - 2.0*k*cos(2.0*pi*(i + rank*n_per_proc)*h);  //u_t (x,0) = -2cos(pi*x)     //u_t (x,0) = 0
    } 
    
    for(int t = 2; t<=n;t++)
    {
        // updating u for time step
        for(int i = 1;i<num-1;i++)
        {
          u[i] = 2*u_old[i] - u_old2[i] + ((k*k*c*c)/(h*h))*(u_old[i+1] - 2*u_old[i] + u_old[i-1]) + k*k*(1.0+((double)t/n)*cos(pi*(rank*n_per_proc + i)*h));
        }
        
        // updating u(t-1) and u(t) for time step
        memcpy(u_old2,u_old,num*sizeof(double));
        memcpy(&u_old[1],&u[1],(num-2)*sizeof(double));
       
        // Sending terminal values in segment 
        if(rank!=0)
            MPI_Send(&u_old[1], 1, MPI_DOUBLE, rank - 1, tag, MPI_COMM_WORLD);
        
        if(rank!=no_proc-1)
            MPI_Send(&u_old[num-2], 1, MPI_DOUBLE, rank + 1, tag, MPI_COMM_WORLD);
       
       // Receiving left terminal values or setting left Boundary Conditions 
        if(rank!=0)
            MPI_Recv(&u_old[0], 1, MPI_DOUBLE, rank - 1, tag, MPI_COMM_WORLD, &stat);
        else
            u_old[0] = u_old[1];			
      
       // Receiving right terminal values or setting right Boundary Conditions 
        if(rank!=no_proc-1)
            MPI_Recv(&u_old[num-1], 1, MPI_DOUBLE, rank + 1, tag, MPI_COMM_WORLD, &stat);
        else
            u_old[num-1] = u_old[num-2];
    }

    printf("\n");
    printf("rank final = %d\t", rank);
    for(int i = 0;i <= 5;i++)
        printf("u__old[%d] = %f\t",rank*n_per_proc + i*(num-1)/5, u_old[i*(num-1)/5]);
    printf("\n");

    free(u_old2);
    free(u_old);
    free(u);
    MPI_Finalize();
}
