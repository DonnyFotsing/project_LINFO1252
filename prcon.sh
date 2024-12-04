#!/bin/bash

echo "Nb_threads,Measure_1,Measure_2,Measure_3,Measure_4,Measure_5" > prod_cons_measures.csv

if [ ! -f prod_cons ]; then
    gcc -o prod_cons prcon.c -pthread
fi

for i in 2 4 8 16 32; do
  #ici on separe le nombre de thread en deux de taille egale donc un pour producteur un pour consommateur
  num_threads_per_type=$((i / 2))
  echo -n "$i," >> prod_cons_measures.csv
  for _ in {1..5}; do
    { time ./prod_cons $num_threads_per_type $num_threads_per_type > /dev/null; } 2> /tmp/measure
    #ici on recupere le temps en forme minute secode puis on la transforme en seconde 
    #puis en on l'imprime en s'assurant de supprimer le retour à la ligne pour imprimer toute les mesures...
    cat /tmp/measure | grep real | awk '{split($2,a,"m|s"); print a[1]*60 + a[2]}' | tr -d "\n" >> prod_cons_measures.csv
    echo -n "," >> prod_cons_measures.csv
  done
  echo "" >> prod_cons_measures.csv
done

rm /tmp/measure

echo "Mesures pour le problème des producteurs-consommateurs enregistrées dans prod_cons_measures.csv"
