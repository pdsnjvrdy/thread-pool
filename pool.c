#include "pool.h"
#include <stdlib.h>
#include <stdio.h>

// worker thread keeps looking for tasks
void* worker(void *arg) {
    thread_pool_t *pool = (thread_pool_t*)arg;

    while (1) {
        pthread_mutex_lock(&pool->lock);

        // wait until there is a task or stop signal
        while (pool->task_head == NULL && !pool->stop) {
            pthread_cond_wait(&pool->cond, &pool->lock);
        }

        if (pool->stop) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        // grab a task
        task_t *task = pool->task_head;
        pool->task_head = task->next;
        if (pool->task_head == NULL) {
            pool->task_tail = NULL;
        }

        pthread_mutex_unlock(&pool->lock);

        // run the task
        task->func(task->arg);
        free(task);
    }

    return NULL;
}

thread_pool_t* pool_create(int num_threads) {
    thread_pool_t *pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));
    if (pool == NULL) {
        printf("malloc failed for pool\n");
        return NULL;
    }

    pool->num_threads = num_threads;
    pool->task_head = NULL;
    pool->task_tail = NULL;
    pool->stop = 0;

    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->cond, NULL);

    pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
    if (pool->threads == NULL) {
        printf("malloc failed for threads array\n");
        free(pool);
        return NULL;
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, worker, (void*)pool);
    }

    return pool;
}

void pool_submit(thread_pool_t *pool, task_func func, void *arg) {
}

void pool_destroy(thread_pool_t *pool) {
}