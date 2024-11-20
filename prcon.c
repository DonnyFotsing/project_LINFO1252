#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 8
#define NUM_ELEMENTS 131072

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t empty;//indique Combien d'element peuvent ecore entrer
sem_t full;//indique combien d'element sont deja present
pthread_mutex_t mutex;

typedef struct {
    int id;//numero de chaque producteurs ou consommateurs
    int total; //nombre totalde producteur de consommateur
} ThreadParam;

void *producer(void *param) {

    ThreadParam *p = (ThreadParam *)param;
    int id = p->id;
    int nbr_productions = NUM_ELEMENTS / p->total;  
    int rest = NUM_ELEMENTS % p->total;  
    if (p->id < rest) {
        nbr_productions++; 
    }
    

    for (int i = 0; i < nbr_productions; i++) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        buffer[in] = id;
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        for (int j = 0; j < 10000; j++); // Simulation de traitement
    }
    return NULL;
}

void *consumer(void *param) {
    ThreadParam *c = (ThreadParam *)param;
    int nbr_consumptions = NUM_ELEMENTS / c->total;
    int rest = NUM_ELEMENTS % c->total;
    if (c->id < rest) {
        nbr_consumptions++;
    }

    for (int i = 0; i < nbr_consumptions; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        int consumed = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        for (int j = 0; j < 10000; j++); // Simulation de traitement
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number of producers> <number of consumers>\n", argv[0]);
        return 1;
    }

    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);

    if (num_producers <= 0 || num_consumers <= 0) {
        fprintf(stderr, "Le nombre de producteurs et de consommateurs doit être supérieur à 0.\n");
        return 1;
    }

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];

    ThreadParam p_params[num_producers];
    ThreadParam c_params[num_consumers];
    for (int i = 0; i < num_producers; i++) {
        p_params[i].id = i;
        p_params[i].total = num_producers;
        pthread_create(&producers[i], NULL, producer, &p_params[i]);
    }
    for (int i = 0; i < num_consumers; i++) {
        c_params[i].id = i;
        c_params[i].total = num_consumers;
        pthread_create(&consumers[i], NULL, consumer, &c_params[i]);
    }
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}
