#ifndef POOL_H
#define POOL_H

#include <pthread.h>

typedef void (*task_func)(void*);

// A single task in the queue
typedef struct task {
    task_func func;
    void *arg;
    struct task *next;
} task_t;

// Thread pool
typedef struct {
    pthread_t *threads;
    int num_threads;
    task_t *task_head;
    task_t *task_tail;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int stop;
} thread_pool_t;

// API 
thread_pool_t* pool_create(int num_threads);
void pool_submit(thread_pool_t *pool, task_func func, void *arg);
void pool_destroy(thread_pool_t *pool);

#endif