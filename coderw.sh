#!/bin/bash

echo "Nb_threads,Measure_1,Measure_2,Measure_3,Measure_4,Measure_5" > le_csv.csv
gcc -o readers_writers rw.c -pthread 
for total_threads in 2 4 8 16 32; do
   
    num_readers=$((total_threads / 2))
    num_writers=$((total_threads / 2))
    echo -n "$total_threads," >> le_csv.csv
    for _ in 1 2 3 4 5; do
        TIME=$( { time ./readers_writers $num_writers $num_readers > /dev/null; } 2>&1 | grep real | awk '{print $2}')
        echo -n "$TIME;" >> le_csv.csv
    done
    
    echo "" >> le_csv.csv
done


