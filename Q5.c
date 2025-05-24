#include <stdio.h>

typedef struct {
    int available; 
} lock;

int compare_and_swap(int *ptr, int expected, int newval) {
    int old = *ptr;
    if (old == expected) {
        *ptr = newval;
    }
    return old;
}

void init_lock(lock *m) {
    m->available = 0;  
}

void acquire(lock *m) {
    while (compare_and_swap(&m->available, 0, 1) != 0) {
       
    }
}

void release(lock *m) {
    compare_and_swap(&m->available, 1, 0);
}

int main() {
    lock mutex;
    init_lock(&mutex);

    acquire(&mutex);
    printf("Thread 1: lock acquired\n");


    release(&mutex);
    printf("Thread 1: lock released\n");

    return 0;
}
