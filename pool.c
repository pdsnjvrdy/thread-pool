#include "pool.h"
#include <stdlib.h>
#include <stdio.h>

// worker thread keeps looking for tasks
void* worker(void *arg) {
    thread_pool_t *pool = (thread_pool_t*)arg;

    while (1) {
        pthread_mutex_lock(&pool->lock);

        // Wait until there is a task or stop signal
        while (pool->task_head == NULL && !pool->stop) {
            pthread_cond_wait(&pool->cond, &pool->lock);
        }

        if (pool->stop && pool->task_head == NULL) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        // grab a task
        task_t *t = pool->task_head;
        pool->task_head = t->next;
        if (pool->task_head == NULL) {
            pool->task_tail = NULL;
        }

        pthread_mutex_unlock(&pool->lock);

        // run the task
        t->func(t->arg);
        free(t);
    }

    return NULL;
}

thread_pool_t* pool_create(int num_threads) {
    thread_pool_t *pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));
    if (pool == NULL) {
        printf("malloc pool failed\n");
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
        printf("malloc threads failed\n");
        free(pool);
        return NULL;
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker, (void*)pool) != 0) {
            printf("thread %d creation failed\n", i);
        }
    }

    return pool;
}

void pool_submit(thread_pool_t *pool, task_func func, void *arg) {
    task_t *t = (task_t*)malloc(sizeof(task_t));
    if (t == NULL) {
        printf("malloc task failed\n");
        return;
    }
    t->func = func;
    t->arg = arg;
    t->next = NULL;

    pthread_mutex_lock(&pool->lock);

    // Add to tail
    if (pool->task_tail == NULL) {
        pool->task_head = t;
        pool->task_tail = t;
    } else {
        pool->task_tail->next = t;
        pool->task_tail = t;
    }

    // Signal one worker that a task is available
    pthread_cond_signal(&pool->cond);

    pthread_mutex_unlock(&pool->lock);
}

void pool_destroy(thread_pool_t *pool) {
    pthread_mutex_lock(&pool->lock);
    pool->stop = 1;
    // Wake all workers so they can exit
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->lock);

    // Join all threads
    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    // Free remaining tasks (shouldn't be any if all workers exited)
    task_t *curr = pool->task_head;
    while (curr != NULL) {
        task_t *next = curr->next;
        free(curr);
        curr = next;
    }

    free(pool->threads);
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->cond);
    free(pool);
}