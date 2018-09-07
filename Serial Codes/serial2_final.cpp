#include<iostream>
#include<math.h>
#include<iomanip>
#include<string.h>
#include<fstream>

using namespace std;

int n = 1000;
double pi = 3.14159265359;
double h=1.0/n, k=1.0/n, c=1;

int main()
{
    double* u = new double[n+1];
    double* u_old = new double[n+1];
    double* u_old2 = new double[n+1];
  
 
    ofstream f;
    f.open("wave2.txt");
    
    for(int i=0; i<=n; i++)
   {
    u_old2[i] = 2;        //u(x,0) = 2
    u_old[i] = u_old2[i] - 2.0*k*cos(2.0*pi*(double)i*h);  //u_t (x,0) = -2cos(pi*x)     //u_t (x,0) = 0
    f<<u_old2[i] << "\t";
    }
    f<< "\n";
    
    for(int t = 2;t<= n;t++)
    { 
   	 
   	 for(int i = 0;i<=n;i++)
   	 	f<< u_old[i]<< "\t";
        for (int i = 1; i < n; ++i)
        {
          u[i] = 2*u_old[i] - u_old2[i] + ((k*k*c*c)/(h*h))*(u_old[i+1] - 2*u_old[i] + u_old[i-1]) + k*k*(1.0+((double)t/n)*cos(pi*i*h));  
        }
        f<<"\n";
        
        u[0] = u[1];              //- h*ux[0][j]
        u[n] = u[n-1];            //+ h*ux[n][j]
        memcpy(u_old2,u_old,(n+1)*sizeof(double));
        memcpy(u_old,u,(n+1)*sizeof(double));
    }
    for(int i = 0;i <=n;i++)
 	f<< u_old[i]<< "\t";
    f<<"\n";

  for(int i=0; i<=10; i++)
  {
      cout<<setprecision(4)<<u[i*(n/10)]<<"\t";
  }

  cout<<endl;
  for(int j=0; j<=10; j++)
  {
    cout<<"\n";
    for(int i=0; i<=10; i++)
    {
      cout<<setprecision(4)<<pow((double)(j*k*n)/10,2)/2 + 2 + ((double)((j*k*n)/10)/pow(pi,2) - (double)sin(pi*(j*k*n)/10)/pow(pi,3))*cos(pi*(i*h*n)/10) - 1.0/pi*sin(2*pi*(j*k*n)/10)*cos(2*pi*(i*h*n)/10)<<"\t";
    }
  }
  cout<<endl;
  delete[] u ;
  delete[] u_old ;
  delete[] u_old2 ;
}
