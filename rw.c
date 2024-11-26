#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Initialisation des mutex et des semaphores
pthread_mutex_t mutex_reader;
pthread_mutex_t mutex_writer;
pthread_mutex_t z;

sem_t db_reader;
sem_t db_writer;

int writercount = 0;
int readcount = 0; // nombre de readers
int total_writes = 0; // nombre total d'écritures
int total_reads = 0;  // nombre total de lectures

#define MAX_WRITES 640
#define MAX_READS 2540

/// @brief Fonction simulant un traitement
void process()
{
    for (int i = 0; i < 10000; i++);
}

/// @brief Fonction de lecture de l'algorithme des lecteurs-écrivains
void* reader(void* arg)
{
    while (total_reads < MAX_READS) // Limiter le nombre total de lectures
    {
        pthread_mutex_lock(&z); // Permet de donner la priorité absolue aux writers

        sem_wait(&db_reader); // Sémaphore permettant aux écrivains de bloquer les lecteurs

        pthread_mutex_lock(&mutex_reader); // Mutex pour protéger la variable readcount
        readcount++;

        if (readcount == 1)
            sem_wait(&db_writer); // premier reader réserve la database pour être sûr qu'aucun writer ne l'utilise

        pthread_mutex_unlock(&mutex_reader);
        sem_post(&db_reader);
        pthread_mutex_unlock(&z);

        // Lecture simulée
        process();
        total_reads++;

        pthread_mutex_lock(&mutex_reader);
        readcount--;
        if (readcount == 0)
            sem_post(&db_writer); // dernier reader libère la database écrivain

        pthread_mutex_unlock(&mutex_reader);
    }
    return NULL;
}

/// @brief Fonction d'écriture de l'algorithme des lecteurs-écrivains
void* writer(void* arg)
{
    while (total_writes < MAX_WRITES) // Limiter le nombre total d'écritures
    {
        pthread_mutex_lock(&mutex_writer); // Mutex pour protéger la variable writercount
        writercount++;
        if (writercount == 1) // arrivée du premier writer
            sem_wait(&db_reader); // bloque les lecteurs
        pthread_mutex_unlock(&mutex_writer);

        sem_wait(&db_writer); // bloque les autres writers et vérifie que la database n'est pas en train d'être lue

        // Écriture simulée
        process();
        total_writes++;

        sem_post(&db_writer); // libère les autres writers

        pthread_mutex_lock(&mutex_writer);
        writercount--;
        if (writercount == 0) // départ du dernier writer
            sem_post(&db_reader); // libère les lecteurs
        pthread_mutex_unlock(&mutex_writer);
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number of writers> <number of readers>\n", argv[0]);
        return 1;
    }
    int num_writers = atoi(argv[1]);
    int num_readers = atoi(argv[2]);
    if (num_writers <= 0 || num_readers <= 0) {
        fprintf(stderr, "Le nombre d'écrivain et de lecteurs doit être supérieur à 0.\n");
        return 1;
    }
    pthread_mutex_init(&mutex_reader, NULL);
    pthread_mutex_init(&mutex_writer, NULL);
    pthread_mutex_init(&z, NULL);
    pthread_t writers[num_writers];
    pthread_t readers[num_readers];
    sem_init(&db_reader, 0, 1);
    sem_init(&db_writer, 0, 1); 
    for (int i = 0; i < num_writers; i++) {
        pthread_create(&writers[i], NULL, writer, NULL);
    }
    for (int i = 0; i < num_readers; i++) {
        pthread_create(&readers[i], NULL, reader, NULL);
    }
    for (int i = 0; i < num_writers; i++) {
        pthread_join(writers[i], NULL);
    }
    for (int i = 0; i < num_readers; i++) {
        pthread_join(readers[i], NULL);
    }

    printf("Terminé : %d écritures et %d lectures effectuées.\n", total_writes, total_reads);
    return 0;
}
