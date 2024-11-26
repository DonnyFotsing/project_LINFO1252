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

sem_t empty; 
sem_t full;
pthread_mutex_t mutex;          
pthread_mutex_t count_mutex; 

int rest_prod = NUM_ELEMENTS; 
int rest_cons = NUM_ELEMENTS; 
typedef struct {
    int id;
} ThreadParam;

//@brief fonction du producteur de l'algorithme procteur-consommateur 
void *producer(void *param) {
    ThreadParam *p = (ThreadParam *)param;
    int id = p->id;
    while (1) {
        pthread_mutex_lock(&count_mutex);
        if (rest_prod <= 0) {
            pthread_mutex_unlock(&count_mutex);
            break;
        }
        rest_prod--;
        pthread_mutex_unlock(&count_mutex);
        sem_wait(&empty); 
        pthread_mutex_lock(&mutex);
        buffer[in] = id;
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        for (int j = 0; j < 10000; j++);
    }
    return NULL;
}

//@brief fonction consommateur de l'algorithe producteur consommmateur 
void *consumer(void *param) {
    ThreadParam *c = (ThreadParam *)param;
    int id = c->id;

    while (1) {
        pthread_mutex_lock(&count_mutex);
        if (rest_cons <= 0) {
            pthread_mutex_unlock(&count_mutex);
            break;
        }
        rest_cons--;
        pthread_mutex_unlock(&count_mutex);
        sem_wait(&full); 
        pthread_mutex_lock(&mutex);
        int consumed = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty); 
        for (int j = 0; j < 10000; j++);
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
    pthread_mutex_init(&count_mutex, NULL);

    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];

    ThreadParam p_params[num_producers];
    ThreadParam c_params[num_consumers];
    for (int i = 0; i < num_producers; i++) {
        p_params[i].id = i;
        pthread_create(&producers[i], NULL, producer, &p_params[i]);
    }
    for (int i = 0; i < num_consumers; i++) {
        c_params[i].id = i;
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
    pthread_mutex_destroy(&count_mutex);

    return 0;
}
