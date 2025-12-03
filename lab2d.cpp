#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3

void* thread_function(void* arg) {
    int id = *((int*)arg);
    printf("Hello from Thread %d (ID: %lu)\n", id, pthread_self());
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, thread_function, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads finished execution.\n");
    return 0;
}
