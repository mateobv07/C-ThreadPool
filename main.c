#include "./ThreadPool.h"


void example_task(void* arg){
    int* num = (int*)arg;
    printf("proccesing task %d\n", *num);
    sleep(2);
    free(arg);
}

int main() {
    threadpool_t pool;
    threadpool_init(&pool);

    for(int i = 0; i < 100; i++){
        int* task_num = malloc(sizeof(int));
        *task_num = i;
        add_task(&pool, example_task, task_num);
    }

    sleep(10);
    threadpool_destroy(&pool);
    return 0;
}
