# 1d-wave-equation
Implementation, Parallelisation and Evaluation of the finite difference method for solving the one dimensional wave equation
**EXECUTING SERIAL CODES**
g++ filname.cpp
>>A file "wave1.txt" is created in the same folder with the solution of the wave equation.

**EXECUTING RESIDUAL**
`g++ filename.cpp`
>>The file "wave1.txt" must be copied into the same folder as the residual file before executing this command.

**EXECUTING MPI CODES**
`mpic++ filename.cpp`
'mpirun -np 2 ./a.out'
>> Running with 2 mpi processes

**EXECUTING OpenCL CODES**
`gcc -lOpenCL filename.c`
