# Variables
CC = gcc
CFLAGS = -pthread -Wall
TARGETS = readers_writers producers_consumers philosophers
SCRIPTS_CSV = coderw.sh bashpr.sh codebash.sh
PLOT_SCRIPTS = gra.py grap.py graphe.py


all: $(TARGETS) csv plots clean

# Compilation des programmes
readers_writers: rw.c
	$(CC) $(CFLAGS) -o readers_writers rw.c

producers_consumers: prcon.c
	$(CC) $(CFLAGS) -o producers_consumers prcon.c

philosophers: projet1.c
	$(CC) $(CFLAGS) -o philosophers projet1.c

# Génération des CSV
csv_readers_writers: readers_writers
	bash coderw.sh

csv_producers_consumers: producers_consumers
	bash bashpr.sh

csv_philosophers: philosophers
	bash codebash.sh

csv: csv_readers_writers csv_producers_consumers csv_philosophers

# Plots
plot_readers_writers: csv_readers_writers
	python3 gra.py readers_writers.csv

plot_producers_consumers: csv_producers_consumers
	python3 grap.py producers_consumers.csv

plot_philosophers: csv_philosophers
	python3 graphe.py philosophers.csv

plots: plot_readers_writers plot_producers_consumers plot_philosophers

# Nettoyage
clean:
	@rm -f $(TARGETS) *.csv *.png
	
