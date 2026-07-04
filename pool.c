#include "pool.h"
#include <stdlib.h>

// worker thread function
void* worker(void *arg) {
    return NULL;
}

thread_pool_t* pool_create(int num_threads) {
    thread_pool_t *pool = malloc(sizeof(thread_pool_t));
    return pool;
}

void pool_submit(thread_pool_t *pool, task_func func, void *arg) {
}

void pool_destroy(thread_pool_t *pool) {
    
}