#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define BUFFER_SIZE 8
#define NUM_ELEMENTS 131072

typedef struct {
    int lock;
} slock_t;

void lock(slock_t *s) {
    int expected;
    do {
        expected = 0;
        asm volatile (
            "xchg %0, %1"
            : "=r"(expected), "=m"(s->lock)
            : "0"(1), "m"(s->lock)
            : "memory"
        );
    } while (expected != 0);
}

void unlock(slock_t *s) {
    asm volatile (
        "movl $0, %0"
        : "=m"(s->lock)
        :
        : "memory"
    );
}

typedef struct {
    int count;
    slock_t lock;
} semaphore_t;

void semaphore_init(semaphore_t *sem, int value) {
    sem->count = value;
    sem->lock.lock = 0;
}

void semaphore_wait(semaphore_t *sem) {
    while (1) {
        lock(&sem->lock);
        if (sem->count > 0) {
            sem->count--;
            unlock(&sem->lock);
            break;
        }
        unlock(&sem->lock);
    }
}

void semaphore_post(semaphore_t *sem) {
    lock(&sem->lock);
    sem->count++;
    unlock(&sem->lock);
}

int buffer[BUFFER_SIZE];
int in = 0, out = 0;
int rest_prod = NUM_ELEMENTS, rest_cons = NUM_ELEMENTS;

semaphore_t empty, full;
slock_t mutex;

void *producer(void *param) {
    while (1) {
        lock(&mutex);
        if (rest_prod <= 0) {
            unlock(&mutex);
            break;
        }
        rest_prod--;
        unlock(&mutex);

        semaphore_wait(&empty);
        lock(&mutex);
        buffer[in] = 1;
        in = (in + 1) % BUFFER_SIZE;
        unlock(&mutex);
        semaphore_post(&full);
    }
    return NULL;
}

void *consumer(void *param) {
    while (1) {
        lock(&mutex);
        if (rest_cons <= 0) {
            unlock(&mutex);
            break;
        }
        rest_cons--;
        unlock(&mutex);

        semaphore_wait(&full);
        lock(&mutex);
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        unlock(&mutex);
        semaphore_post(&empty);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 1;
    }

    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);

    semaphore_init(&empty, BUFFER_SIZE);
    semaphore_init(&full, 0);
    mutex.lock = 0;

    pthread_t producers[num_producers], consumers[num_consumers];

    for (int i = 0; i < num_producers; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}
