#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define THREADS 8
#define QUEUE_SIZE 100

typedef struct {
    void (*fn)(void* arg);
    void* arg;
} task_t;

 typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t notify; //  A condition variable to notify threads when a new task is available.
    pthread_t threads[THREADS];
    task_t task_queue[QUEUE_SIZE];
    int queue_size;  // How many tasks are in the queue;
    int queue_front; // Next element to be dispatched. The index of the front of the task queue.
    int queue_back;  // What was the last element dispatched. The index of the rear of the task queue.
    int stop;
} threadpool_t;

void* thread_function(void* threadpool){
    threadpool_t* pool = (threadpool_t*)threadpool;

    while(1) {
        pthread_mutex_lock(&(pool->lock));
    
        while(pool->queue_size == 0 && !pool->stop) {
            // Sleep thread until condition becomes true.
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if (pool->stop) {
            pthread_mutex_unlock(&(pool->lock));
            pthread_exit(NULL);
        }

        task_t task = pool->task_queue[pool->queue_front];
        pool->queue_front = (pool->queue_front + 1) % QUEUE_SIZE;
        pool->queue_size --;

        pthread_mutex_unlock(&(pool->lock));
        // Execute task.
        (*(task.fn))(task.arg);
    }
    return NULL;
}

void threadpool_init(threadpool_t* pool){
    pool->queued = 0;
    pool->queue_front = 0;
    pool->queue_back = 0;
    pool->stop;
    
    pthread_mutex_init(&(pool->lock), NULL);
    pthrad_cond_init(&(pool->notify), NULL);

    for (int i = 0; i < THREADS; i++) {
        pthread_create(&(pool->threads[i]), NULL, thread_function, pool);
    }
}