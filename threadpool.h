/*
* MIT License
*
* Copyright (c) 2025 Stefan Mladic
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdbool.h>

#define MAX_THREADS 64
// 2^16 ensures cache-friendly alignment 
#define MAX_QUEUE 65536
/**
* task_t - Work unit for thread queue
* @task: Function pointer to execute the work
* @argument: Generic argument passed to the task function
* @cleanup: Optional cleanup function for argument memory management
* 
* NOTE: For multiple arguments, pack them into a struct and allocate on heap
* for memory safety across thread boundaries. Use cleanup to free resources.
*/
typedef struct {
  void (*task)(void *);
  void *argument;
  void (*cleanup)(void *);
} task_t;
/**
* task_queue_t - Thread-safe circular buffer for task storage
* @tasks: Dynamic array of tasks (circular buffer)
* @head: Index where consumers (workers) read from
* @tail: Index where producers write to
* @count: Current number of tasks in queue
* @capacity: Maximum queue capacity
* @mutex: Protects all queue operations
* @not_full: Condition variable for producers waiting on queue space
* @not_empty: Condition variable for consumers waiting on available tasks
*/
typedef struct {
  task_t *tasks;
  size_t head, tail, count, capacity;
  pthread_mutex_t mutex;
  pthread_cond_t not_full;
  pthread_cond_t not_empty;
} task_queue_t;
/**
* threadpool_t - Main threadpool structure
* @shutdown: Volatile flag for safe shutdown signaling across threads
* @workers: Array of worker thread handles
* @thread_count: Number of active worker threads
* @queue: Pointer to the shared task queue
*/
typedef struct {
  volatile bool shutdown;
  pthread_t *workers;
  size_t thread_count;
  task_queue_t *queue;
} threadpool_t;
/**
* threadpool_create - Initialize a new threadpool
* @thread_count: Number of worker threads to spawn
* @queue_size: Maximum number of pending tasks in queue
* 
* Returns: Pointer to threadpool on success, NULL on failure
*/
threadpool_t *threadpool_create(size_t thread_count, size_t queue_size);
/**
* threadpool_add - Submit work to the threadpool
* @pool: Target threadpool
* @task: Task structure containing function, args, and cleanup
* 
* Returns: 0 on success, error code on failure
* NOTE: Blocks if queue is full until space becomes available
*/
int threadpool_add(threadpool_t *pool, const task_t *task);
/**
* threadpool_destroy - Shutdown and cleanup threadpool
* @pool: Threadpool to destroy
* @graceful: If true, processes remaining queued tasks; if false, discards queue
* 
* Returns: 0 on success, error code on failure
* NOTE: Always blocks until all worker threads have joined
*/
int threadpool_destroy(threadpool_t *pool, bool graceful);
#ifdef __cplusplus
}
#endif

#endif
