Go to directory `apm_parallel`

### To compile
```
  mpicc -o apm_<type> src/apm_<type>.c 
```

### To run
```
mpirun -np 4 ./apm_<type> 0 <path_to_dna>/<file_name>.fa <pattern_1> <pattern_2> <pattern_3> ...
```
