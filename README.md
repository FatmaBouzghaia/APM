Go to directory `apm_parallel`

### To compile
```
  mpicc -o apm_<type> src/apm_<type>.c 
```

### To run
```
mpirun -np 4 ./apm_<type> 0 <path_to_dna>/<file_name>.fa <pattern_1> <pattern_2> <pattern_3> ...
```

### EZtrace

#### OpenMP
```
make CC="eztrace_cc gcc"
eztrace -t omp ./apm_<type> 0 <path_to_dna>/<file_name> <pattern_1> <pattern_2> <pattern_3> ...
```

### MPI
```
mpirun -np <nb_threads> eztrace -t mpi ./apm_<type> 0 <path_to_dna>/<file_name> <pattern_1> <pattern_2> <pattern_3> ...
```
**To See The EZtrace**
```
eztrace_convert ****_log_rank_1
vite eztrace_output.trace
```
