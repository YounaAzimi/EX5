#include <stdio.h>
#include <semaphore.h> 
#define MAX_PARTICLES 100  
typedef struct {
    int id;  
} particle;

particle buffer[MAX_PARTICLES];
int head = 0, tail = 0;   


sem_t empty_pairs;  // init 50
sem_t full_pairs;   // init 0
sem_t mutex;        // init 1

void init() {
    sem_init(&empty_pairs, 0, 50);
    sem_init(&full_pairs,  0, 0);
    sem_init(&mutex,       0, 1);
}

void Producer() {
    particle P1, P2;
    while (1) {
        sem_wait(&empty_pairs);
        sem_wait(&mutex);
        buffer[tail] = P1;
        tail = (tail + 1) % MAX_PARTICLES;
        buffer[tail] = P2;
        tail = (tail + 1) % MAX_PARTICLES;
        sem_post(&mutex);
        sem_post(&full_pairs);
    }
}

void Consumer() {
    particle P1, P2;
    while (1) {
        sem_wait(&full_pairs);

        sem_wait(&mutex);

        P1 = buffer[head];
        head = (head + 1) % MAX_PARTICLES;
        P2 = buffer[head];
        head = (head + 1) % MAX_PARTICLES;
        sem_post(&mutex);
        sem_post(&empty_pairs);
        PackageAndShip(P1, P2);
    }
}
