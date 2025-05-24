#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_SEATS    5    // number of waiting chairs
#define MAX_STUDENTS 20   // total number of students

// shared data
sem_t customerReady;     // counts waiting students
sem_t taReady;           // signals TA is ready for next student
pthread_mutex_t seatMutex;
int availableSeats = MAX_SEATS;
int studentsServed = 0;
int totalStudents;

// student thread function
void* student(void* arg) {
    int id = *(int*)arg;
    free(arg);

    // simulate random arrival time
    usleep((rand() % 5 + 1) * 100000);  // 0.1–0.5 seconds

    printf("Student %d: arrived.\n", id);

    // try to sit in waiting area
    pthread_mutex_lock(&seatMutex);
    if (availableSeats > 0) {
        availableSeats--;
        printf("Student %d: took a seat. Seats left: %d\n",
               id, availableSeats);
        pthread_mutex_unlock(&seatMutex);

        // notify TA and wait for service
        sem_post(&customerReady);
        sem_wait(&taReady);

        // being helped
        printf("Student %d: being helped by TA.\n", id);
    } else {
        // no seats available
        printf("Student %d: no seats available, will try later.\n", id);
        pthread_mutex_unlock(&seatMutex);
    }

    return NULL;
}

// TA thread function
void* ta(void* arg) {
    (void)arg;
    while (1) {
        // stop when all students have been served
        if (studentsServed >= totalStudents) break;

        // wait for a student
        sem_wait(&customerReady);

        // signal a student to come for help
        sem_post(&taReady);

        // help the student
        printf("TA: helping a student...\n");
        sleep(1);

        // free up a seat
        pthread_mutex_lock(&seatMutex);
        availableSeats++;
        studentsServed++;
        printf("TA: done helping. Seats available: %d. Served %d/%d\n",
               availableSeats, studentsServed, totalStudents);
        pthread_mutex_unlock(&seatMutex);
    }

    printf("TA: all students helped, closing office.\n");
    return NULL;
}

int main() {
    srand(time(NULL));
    totalStudents = MAX_STUDENTS;

    // initialize semaphores and mutex
    sem_init(&customerReady, 0, 0);
    sem_init(&taReady,      0, 0);
    pthread_mutex_init(&seatMutex, NULL);

    // create TA thread
    pthread_t taThread;
    pthread_create(&taThread, NULL, ta, NULL);

    // create student threads
    pthread_t studentThreads[MAX_STUDENTS];
    for (int i = 0; i < MAX_STUDENTS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&studentThreads[i], NULL, student, id);
    }

    // wait for all student threads
    for (int i = 0; i < MAX_STUDENTS; i++) {
        pthread_join(studentThreads[i], NULL);
    }
    // wait for TA thread
    pthread_join(taThread, NULL);

    // destroy semaphores and mutex
    sem_destroy(&customerReady);
    sem_destroy(&taReady);
    pthread_mutex_destroy(&seatMutex);

    return 0;
}
