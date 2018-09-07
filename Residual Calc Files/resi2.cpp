#include<iostream>
#include<fstream>
#include<math.h>
using namespace std;

int n = 1000;
double pi = 3.14159265359;
double h = 1.0/n, k = 1.0/n, c = 1.0;

double l1 (double e[1001][1001])
{
  double sum = 0;
  for(int i=1; i<n; i++)
    for(int j=1; j<n; j++)
      sum += abs(e[i][j]);
  for(int i=1; i<n; i++)
    sum += 0.5*(abs(e[0][i]) + abs(e[n][i]) + abs(e[i][0]) + abs(e[i][n]));
  sum += 0.25*(abs(e[0][0]) + abs(e[0][n]) + abs(e[n][0]) + abs(e[n][n]));
  return sum/(n*n);
}

double l2 (double e[1001][1001])
{
  double sum = 0;
  for(int i=1; i<n; i++)
    for(int j=1; j<n; j++)
      sum += pow((e[i][j]),2);
  for(int i=1; i<n; i++)
    sum += 0.5*(pow((e[0][i]),2) + pow((e[n][i]),2) + pow((e[i][0]),2) + pow((e[i][n]),2));
  sum += 0.25*(pow((e[0][0]),2) + pow((e[0][n]),2) + pow((e[n][0]),2) + pow((e[n][n]),2));
  return sqrt(sum/(n*n));
}

double l_infi (double e[1001][1001])
{
  double max = -1;
  for(int i=0; i<=n; i++)
    for(int j=0; j<=n; j++)
      if(abs(e[i][j]) > max)
        max = abs(e[i][j]);
  return max;
}

void residual ()
{
  double x, t, z;
  int col_num = 0, row_num = 0;
  ifstream infile;
  infile.open("wave2.txt");
  double e[1001][1001];
  while (!(row_num == n && col_num == n+1))
  {
    infile>>x;
    if(col_num == n+1)
    {
      row_num++;
      col_num = 0;
    }
    z = col_num*k;
    t = row_num*h;
    e[row_num][col_num] = 0.5*t*t + 2.0 + ((t/(pi*pi)) - sin(pi*t)/pow(pi,3))*cos(pi*z) - (sin(2*pi*t)*cos(2*pi*z))/pi - x;
    col_num++;
  }
  infile.close();
  
  cout<<l1(e)<<endl;
  cout<<l2(e)<<endl;
  cout<<l_infi(e)<<endl;
}

int main()
{
  residual();
  cout<<endl;
}
