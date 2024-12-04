#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define nbr_cycles 1000000
int philosophers; // Nombre de philosophes
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

slock_t *bread; // Baguettes

void *philo(void *param) {
    int n_philo = *(int *)param;
    for (int i = 0; i < nbr_cycles; i++) {
        if (n_philo % 2 == 0) {
            lock(&bread[n_philo]);
            lock(&bread[(n_philo + 1) % philosophers]);
        } else {
            lock(&bread[(n_philo + 1) % philosophers]);
            lock(&bread[n_philo]);
        }
        printf("Philosophe %d mange\n", n_philo);
        unlock(&bread[n_philo]);
        unlock(&bread[(n_philo + 1) % philosophers]);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    philosophers = atoi(argv[1]);
    bread = malloc(philosophers * sizeof(slock_t));
    for (int i = 0; i < philosophers; i++) {
        bread[i].lock = 0;
    }

    pthread_t threads[philosophers];
    int ids[philosophers];

    for (int i = 0; i < philosophers; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philo, &ids[i]);
    }
    for (int i = 0; i < philosophers; i++) {
        pthread_join(threads[i], NULL);
    }

    free(bread);
    return 0;
}
