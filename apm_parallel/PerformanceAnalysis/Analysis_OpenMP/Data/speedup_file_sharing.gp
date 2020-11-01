set terminal png
set ylabel "Speedup"
set xlabel "Number of cores"
set xtics 1
set title "APM Speedup for OpenMP execution"
set output "SpeedUpAPM-OpenMP-fileSharing.png"

plot x title 'Speedup max' with lines, \
     'OMP_file_sharing_small.data' using 1:($2/$3) title 'APM - OpenMP : File Parallelism' with linespoints;