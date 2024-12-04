#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
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

slock_t mutex_reader = {0}, mutex_writer = {0}, z = {0};
semaphore_t db_reader, db_writer;

int readcount = 0, writercount = 0;
int total_reads = 0, total_writes = 0;
#define MAX_READS 2540
#define MAX_WRITES 640

void process() {
    for (int i = 0; i < 10000; i++);
}

void *reader(void *arg) {
    while (total_reads < MAX_READS) {
        lock(&z);
        semaphore_wait(&db_reader);
        lock(&mutex_reader);
        readcount++;
        if (readcount == 1)
            semaphore_wait(&db_writer);
        unlock(&mutex_reader);
        semaphore_post(&db_reader);
        unlock(&z);

        process();
        total_reads++;

        lock(&mutex_reader);
        readcount--;
        if (readcount == 0)
            semaphore_post(&db_writer);
        unlock(&mutex_reader);
    }
    return NULL;
}

void *writer(void *arg) {
    while (total_writes < MAX_WRITES) {
        lock(&mutex_writer);
        writercount++;
        if (writercount == 1)
            semaphore_wait(&db_reader);
        unlock(&mutex_writer);

        semaphore_wait(&db_writer);
        process();
        total_writes++;
        semaphore_post(&db_writer);

        lock(&mutex_writer);
        writercount--;
        if (writercount == 0)
            semaphore_post(&db_reader);
        unlock(&mutex_writer);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 1;
    }

    int num_writers = atoi(argv[1]);
    int num_readers = atoi(argv[2]);

    semaphore_init(&db_reader, 1);
    semaphore_init(&db_writer, 1);

    pthread_t writers[num_writers], readers[num_readers];

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

    return 0;
}
