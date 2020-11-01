set terminal png
set ylabel "Speedup"
set xlabel "Number of cores"
set xtics 1
set title "APM Speedup for OpenMP execution"
set output "SpeedUpAPM-OpenMP-patternSharing.png"

plot x title 'Speedup max' with lines, \
     'OMP_pattern_sharing.data' using 1:($2/$3) title 'APM - OpenMP : Pattern Parallelism' with linespoints, \
     'OMP_pattern_sharing_schDynamic.data' using 1:($2/$3) title 'APM - OpenMP : Pattern Parallelism with Dynamic Scheduling' with linespoints;