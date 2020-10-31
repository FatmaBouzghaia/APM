#!/bin/bash 

export GOMP_CPU_AFFINITY=`seq -s"," 0 47`
DNA_FILE="../dna/human_dna/chr11.fa"
DNA_PATTERN="GCC"
DISTANCE=0
t_seq=""

for i in $(seq 1 48) ; do
    t_parallel=$(mpirun -np $i ./apm_mpi $DISTANCE $DNA_FILE $DNA_PATTERN  |grep  APM | awk '{print $4}')
    if [ $i -eq  1 ]; then
        t_seq=$t_parallel
    fi
    echo -e "$i\t$t_seq\t$t_parallel" 
done

