#include "pool.h"
#include <stdio.h>
#include <pthread.h>

// shared data for all tasks
typedef struct {
    int *counter;
    pthread_mutex_t *lock;
} counter_data_t;

void increment_task(void *arg) {
    counter_data_t *data = (counter_data_t*)arg;
    pthread_mutex_lock(data->lock);
    (*data->counter)++;
    pthread_mutex_unlock(data->lock);
}

int main() {
    int counter = 0;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    counter_data_t data;
    data.counter = &counter;
    data.lock = &lock;

    thread_pool_t *pool = pool_create(4);
    if (pool == NULL) {
        printf("pool create failed\n");
        return 1;
    }

    int num_tasks = 100;
    for (int i = 0; i < num_tasks; i++) {
        pool_submit(pool, increment_task, &data);
    }

    // immediately destroy, no sleep
    pool_destroy(pool);

    // all tasks should have been drained
    printf("Expected: %d, Got: %d\n", num_tasks, counter);
    if (counter == num_tasks) {
        printf("All tasks drained correctly.\n");
    } else {
        printf("ERROR: missed %d tasks.\n", num_tasks - counter);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}