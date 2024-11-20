//implementation du problème du philosophe 
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define nbr_cycles 1000000 
int philosophers; //nbr de philo...
pthread_mutex_t *bread;

void *philo(void *param) {
    int n_philo = *(int*)param;//numero ou identifiant de chaque philo...
    for (int i = 0; i < nbr_cycles; i++) {       
        if (n_philo % 2 == 0) {
            //si identifiant paire gauche puis droite..
            pthread_mutex_lock(&bread[n_philo]);
            pthread_mutex_lock(&bread[(n_philo + 1) % philosophers]);
        } else {  
            //sinon droite puis gauche...
            pthread_mutex_lock(&bread[(n_philo + 1) % philosophers]);
            pthread_mutex_lock(&bread[n_philo]);
        }
        printf("Philosophe %d mange\n", n_philo);
        //puis libere apres avoir mangé
        pthread_mutex_unlock(&bread[n_philo]);
        pthread_mutex_unlock(&bread[(n_philo + 1) % philosophers]);

        printf("Philosophe %d a fini de manger\n", n_philo);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <numberofphilosophers>\n", argv[0]);
        return 1;
    }
    philosophers = atoi(argv[1]);//nbr de philo pris en entré
    bread = malloc(philosophers * sizeof(pthread_mutex_t)); //on cree donc un tableau de mutex(baguettes)
    for (int i = 0; i < philosophers; i++) {
        pthread_mutex_init(&bread[i], NULL); //initialisation de mutex
    }
        pthread_t t_philosophers[philosophers]; // tableau de threads representant les philo
    int num[philosophers];

    for (int i = 0; i < philosophers; i++) {
        num[i] = i;//chause philo reçois son identifiant
        pthread_create(&t_philosophers[i], NULL, philo, &num[i]);
    }
        for (int i = 0; i < philosophers; i++) {
        pthread_join(t_philosophers[i], NULL);
    }
    for (int i = 0; i < philosophers; i++) {
        pthread_mutex_destroy(&bread[i]);
    }
    free(bread);

    return 0;
}
