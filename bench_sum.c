#include "pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_TASKS 10000
#define ARRAY_SIZE 1000000   // 1 million

// real CPU work: sum a big array (same array for all tasks)
void sum_array(void *arg) {
    int *arr = (int*)arg;
    long sum = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum += arr[i];
    }
    // prevent compiler from optimising the loop away
    if (sum == -1) printf("impossible\n");
}

// wrapper for naive thread creation
typedef struct {
    int *arr;
} work_arg_t;

void* naive_worker(void *arg) {
    work_arg_t *w = (work_arg_t*)arg;
    sum_array(w->arr);
    return NULL;
}

double get_time_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main() {
    // prepare a single large array (shared by all tasks)
    int *array = (int*)malloc(sizeof(int) * ARRAY_SIZE);
    if (array == NULL) {
        printf("malloc array failed\n");
        return 1;
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i % 100;
    }

    // --- pool benchmark ---
    thread_pool_t *pool = pool_create(8);
    double start = get_time_sec();

    for (int i = 0; i < NUM_TASKS; i++) {
        pool_submit(pool, sum_array, array);
    }

    pool_destroy(pool);
    double end = get_time_sec();
    double pool_time = end - start;

    printf("Pool version: %.3f seconds\n", pool_time);

    // --- naive thread benchmark ---
    start = get_time_sec();

    for (int i = 0; i < NUM_TASKS; i++) {
        pthread_t th;
        work_arg_t arg;
        arg.arr = array;
        pthread_create(&th, NULL, naive_worker, &arg);
        pthread_join(th, NULL);
    }

    end = get_time_sec();
    double naive_time = end - start;

    printf("Naive threads: %.3f seconds\n", naive_time);

    double speedup = naive_time / pool_time;
    printf("Speedup: %.2fx\n", speedup);

    free(array);
    return 0;
}