#include "threadpool.h"
#include <stdlib.h>

static task_queue_t *queue_create(size_t capacity);
static int queue_push(task_queue_t *queue, task_t task);
static int queue_pop(task_queue_t *queue, task_t *task);
static inline bool queue_is_empty(task_queue_t *queue);
static inline bool queue_is_full(task_queue_t *queue);
static void queue_destroy(task_queue_t *queue);
static void *worker_thread(void *arg);

static task_queue_t *queue_create(size_t capacity) {

    if (capacity > MAX_QUEUE || capacity <= 0) {
        return NULL;
    }

    task_queue_t *queue = malloc(sizeof(task_queue_t));
    if (!queue) {
        return NULL;
    }

    queue->tasks = malloc(sizeof(task_t) * capacity);
    if (!queue->tasks) {
        free(queue);
        return NULL;
    }

    queue->head = queue->tail = queue->count = 0;
    queue->capacity = capacity;
    // three seperate checks cus calling pthread destroy funcs failed initalization can cause undefined behaviour
    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        free(queue->tasks);
        free(queue);
        return NULL;
    }
    if (pthread_cond_init(&queue->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&queue->mutex);
        free(queue->tasks);
        free(queue);
        return NULL;
    }
    if (pthread_cond_init(&queue->not_full, NULL) != 0) {
        pthread_mutex_destroy(&queue->mutex);
        pthread_cond_destroy(&queue->not_empty);
        free(queue->tasks);
        free(queue);
        return NULL;
    }

    return queue;
}

static inline bool queue_is_empty(task_queue_t *queue) {
    return queue->count == 0;
}

static inline bool queue_is_full(task_queue_t *queue) {
    return queue->count == queue->capacity;
}

static void queue_destroy(task_queue_t *queue){
    if(!queue) return;

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    free(queue->tasks);
    free(queue);
}

static int queue_push(task_queue_t *queue, task_t task){
    if (!queue) return -1;

    pthread_mutex_lock(&queue->mutex);

    while(queue_is_full(queue)){
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }

    queue->tasks[queue->tail] = task;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->count++;

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

static int queue_pop(task_queue_t *queue, task_t *task) {
    if (!queue || !task) return -1;

    pthread_mutex_lock(&queue->mutex);

    while (queue_is_empty(queue)) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    *task = queue->tasks[queue->head];

    queue->head = (queue->head + 1) % queue->capacity;
    queue->count--;

    pthread_cond_signal(&queue->not_full);

    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

static void *worker_thread(void *arg){
    //cast for pthread_create cus it's generic
    threadpool_t *pool = (threadpool_t *) arg;
    task_t task;

    while (1) {
        if (pool->shutdown) {
            break;
        }
        if (queue_pop(pool->queue, &task) != 0) {
            continue;
        }
        if (task.task) {
            task.task(task.argument);
        }
        if (task.cleanup) {
            task.cleanup(task.argument);
        }
    } 
    pthread_exit(NULL);
}
