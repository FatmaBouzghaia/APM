#!/bin/bash

#MPI
mpicc -o apmMPI src/apm_mpi.c 

#OpenMP File Sharing
gcc -fopenmp src/apm_openMP_file_sharing.c -o apmOpenMP2

#OpenMP Pattern Sharing
gcc -fopenmp src/apm_openMP_pattern_sharing.c -o apmOpenMP1

#OpenMP File Sharing + Dynamic Scheduling
gcc -fopenmp src/apm_openMP_scheduling_dynamic.c -o apmOpenMPSchDynamic

