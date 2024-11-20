#!/bin/bash
echo "Nb_threads,Measure_1,Measure_2,Measure_3,Measure_4,Measure_5" > measures.csv

# Compilation
gcc -o philosopher philosophers.c -pthread || { echo "Compilation failed!"; exit 1; }
for i in 2 4 8 16 32; do
    echo -n "Threads: $i"
    echo -n "$i," >> measures.csv
    for _ in 1 2 3 4 5; do
        TIME=$( { time ./philosopher $i > /dev/null; } 2>&1 | grep real | awk '{print $2}')
        echo -n "$TIME;" >> measures.csv
    done
    echo "" >> measures.csv
done

# Message de fin
echo "Mesures collectées et enregistrées dans measures.csv"
