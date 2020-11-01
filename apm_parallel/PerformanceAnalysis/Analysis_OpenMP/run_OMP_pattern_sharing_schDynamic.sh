#!/bin/bash 

export GOMP_CPU_AFFINITY=`seq -s"," 0 47`
DISTANCE=0
DNA_FILE="../dna/small_chrY.fa"
DNA_PATTERN="GCC ATT AAA GGG CCC TAT ATC GTC TTT CGT"
t_seq=""

cd ..
cd ..

for i in $(seq 1 48) ; do
    t_parallel=$(./apm_OpenMP_pattern_schDynamic $i $DISTANCE $DNA_FILE $DNA_PATTERN  | grep APM | awk '{print $4}')
    if [ $i -eq  1 ]; then
        t_seq=$t_parallel
    fi
    echo -e "$i\t$t_seq\t$t_parallel" 
done

