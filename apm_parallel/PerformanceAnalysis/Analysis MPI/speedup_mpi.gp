set terminal png
set ylabel "speedup"
set xlabel "number of cores"
set xtics 1
set title "APM Speedup for MPI execution"
set output "SpeedUpAPM-MPI.png"

plot x title 'Speedup max' with lines, \
     'run_mpi.data' using 1:($2/$3) title 'APM - MPI' with linespoints;
