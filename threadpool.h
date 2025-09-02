#pragma once

#ifndef THREADPOOL_H
#define THREADPOOL_H
#endif

#ifdef __cplusplus
extern "C"
#endif

#define MAX_THREADS 64
#define MAX_QUEUE 65536

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * Task struct
 * Function is a generic function pointer to be performed by the thread
 * Argument is generic pointer to the Argument passed into the function
 */
typedef struct {
   void (*function)(void *);
   void *argument;
} cpool_task_t;

/*
 * Threadpool
 * lock - mutex for queue
 * threads - pointer to all pthread_t's made
 * queue - functions to be done
 *
 */
typedef struct {
    pthread_mutex_t lock;
    pthread_t *threads;
    pthread_cond_t notify;
    cpool_task_t *queue;
    int thread_count;
    int queue_size;
    int started;
    int shutdown;

} threadpool_t;

int threadpool_free(threadpool_t *pool);

static void *threadpool_thread(void *threadpool);

threadpool_t *cpool_create(int thread_count, int queue_size)
{
    threadpool_t *pool;

    if(thread_count <= 0 || thread_count > MAX_THREADS || queue_size <= 0|| queue_size > MAX_QUEUE){
        return NULL;
    }

    if ((pool = (threadpool_t*)malloc(sizeof(threadpool_t))) == NULL){
        goto err;
    }

    pool->thread_count = 0;
    pool->queue_size = queue_size;
    pool->thread_count = thread_count;
    pool->shutdown = pool->started = 0;

    if ((pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count)) == NULL){
        goto err;
    }
    if ((pool->queue = (cpool_task_t *)malloc(sizeof(cpool_task_t) * queue_size)) == NULL){
        goto err;
    }
    // Initialize mutex and conditional (efficient callback for awaiting threads)
    if ((pthread_mutex_init(&pool->lock, NULL) != 0) ||
        (pthread_cond_init(&(pool->notify), NULL) !=0)){
        goto err;
    }

    for (int i = 0; i < thread_count; i++){
        // todo: implement lol
        // if (pthread_create(&(pool->threads[i], NULL,threadpool_thread, void* pool)) )
        if (1){
            // DONT FORGET TO FREE
            return NULL;
        }
        pool->thread_count++;
        pool->started++;
    }

    return pool;

    err:
    if (pool){
        threadpool_free(pool);
    }
    return NULL;
}

int threadpool_free(threadpool_t *pool)
{
    if(pool == NULL || pool->started < 0){
        return -1;
    }

    if (pool->threads){
        free(pool->threads);
        free(pool->queue);

        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->notify));
    }
    free(pool);
    return 0;
}
