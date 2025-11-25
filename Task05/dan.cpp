#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <csignal>

int honeyPot = 0;
int numbOfBees;
int maxPortions;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pot_empty_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pot_full_cond = PTHREAD_COND_INITIALIZER;

void *bee(void *arg) {
    int bee_id = *((int *) arg);
    while (1) {
        pthread_mutex_lock(&mutex);
        printf("Bee %d flew to flower\n", bee_id);
        sleep(1);
        if (honeyPot == maxPortions) {
            pthread_cond_wait(&pot_empty_cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
        if (honeyPot < maxPortions) {
            honeyPot++;
            sleep(1);
            printf("Bee %d collected one portion of honey. Honey portions in pot: %d\n", bee_id, honeyPot);
        }
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
        if (honeyPot == maxPortions) {
            printf("Bee %d woke up the bear!\n", bee_id);
            pthread_cond_signal(&pot_full_cond);
        }
        pthread_mutex_unlock(&mutex);
    }
}

void *bear(void *arg) {
    int eaten_potions = 0;
    while (1) {
        pthread_mutex_lock(&mutex);
        if (honeyPot < maxPortions) {
            pthread_cond_wait(&pot_full_cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
        while (honeyPot != 0) {
            eaten_potions = std::rand() % honeyPot + 1;
            if (std::rand() % 2 == 0) {
                honeyPot -= eaten_potions;
                printf("Bear ate one portion of honey. Honey portions left: %d\n", honeyPot);
                sleep(1);
            } else {
                printf("Bear chose to have sunbathing\n");
                sleep(1);
            }
        }
        printf("Bear ate all the honey and fell asleep.\n");
        pthread_cond_broadcast(&pot_empty_cond);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("invalid number of arguments");
        return 1;
    }
    numbOfBees = atoi(argv[1]);
    maxPortions = atoi(argv[2]);
    if (numbOfBees <= 0 || maxPortions <= 0) {
        fprintf(stderr, "number of bees or portions can't be zero or less");
        exit(1);
    }
    pthread_t bear_thread;
    pthread_t bee_threads[numbOfBees];

    if (pthread_create(&bear_thread, NULL, bear, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    int bee_ids[numbOfBees];
    for (int i = 0; i < numbOfBees; i++) {
        bee_ids[i] = i + 1;
        if (pthread_create(&bee_threads[i], NULL, bee, &bee_ids[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    pthread_join(bear_thread, NULL);
    for (int i = 0; i < numbOfBees; i++) {
        pthread_join(bee_threads[i], NULL);
    }

    return 0;
}


