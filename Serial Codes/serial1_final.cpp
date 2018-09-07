#include<iostream>
#include<fstream>
#include<math.h>
#include<iomanip>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

using namespace std;

int n = 10;
double h=1.0/n, k=1.0/n, c=1.0;

int main()
{
    /**
     * Creating vectors for u(x) at t-1,t-2 and t
     */
    double* u_old = new double[n+1];
    double* u_old2 = new double[n+1];
    double* u = new double[n+1];
    ofstream f;
    f.open("wave1.txt");
    if(!f.is_open())
       std::cout <<"not open!!\n"; 
    /**
     * Setting initial conditions with du(x,0)/dt = 0 
     */
    for(int i=0; i<=n; i++)
    {
        u_old2[i] = pow(double(i*h),2);	        		//u(x,0) = x^2
        u_old[i] = u_old2[i];					//u_t (x,0) = 0
          //f<<u_old[i]<<"\t";
    }
    
    /**
     * Finite difference in x and t.
     */
    for(int t = 1;t <= n; t++)		//kamal kamal kamal kamal
    {
        for(int i = 0;i<=n;i++)
            f<<u_old[i]<<"\t";
        f<<"\n";
        if(t%1000 == 0)
            std::cout << "t : " << t << std::endl;
        u[0] = pow(double(t*k),2);				//u(0,t) = t^2
        u[n] = 1 + pow(double(t*k),2);				//u(1,t) = 1 + t^2
        for(int i = 1; i < n; i++)
        {
           u[i] = 2*u_old[i] - u_old2[i] + ((k*k*c*c)/(h*h))*(u_old[i+1] - 2*u_old[i] + u_old[i-1]);
        }
        
        memcpy(u_old2,u_old,(n+1)*sizeof(double));
        memcpy(u_old,u,(n+1)*sizeof(double));
        
    }
    
  for(int i=0; i<=n; i++)
  	f<<u_old[i]<<"\t";

  f.close();
  cout<<endl;
  delete [] u;
  delete [] u_old;
  delete [] u_old2;
    return 0;
}
