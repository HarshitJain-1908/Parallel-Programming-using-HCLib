#include "quill-runtime.h"
#include "quill.h"
#include <pthread.h>
#include <cstring>
#include <stdio.h>
#include "iostream"

using namespace quill_runtime;

struct pthread_args{
    int id;
};

// typedef struct{
//     std::function<void()>* work;
// }task_ds;

volatile bool shutdown = false;
volatile int finish_counter = 0;
pthread_key_t ws_key;
pthread_t *thread;
pthread_mutex_t _lock;
// task_ds **deque;
quill_deque **deque;
int *head, *tail;

void *quill_runtime::worker_routine(void *args){
    printf("In worker_routine\n");
    
    //assigns id to the thread
    int id = ((pthread_args *)args)->id;
    pthread_setspecific(ws_key, (void *)&id);

    while (!shutdown){break;
        // printf("hello bro\n");
        // find_and_execute_task();
    }
    return NULL;
}

int quill_runtime::thread_pool_size(){
    // int num_workers= atoi(getenv("QUILL_WORKERS"));
    // return num_workers;
    return 1;
}

void quill_runtime::lock_finish(){
    pthread_mutex_lock(&_lock);
}

void quill_runtime::unlock_finish(){
    pthread_mutex_unlock(&_lock);
}

// void quill_runtime::push_task_to_runtime(quill_deque *deque, quill_task *ptr){
//     // printf("pushing task\n");
//     if (*head == -1) {
//         //deque empty
//         *head = 0;
//     }
//     // memcpy((void *)&deque[++tail].work, ptr, sizeof(std::function<void()>));
//     // deque[++tail].work = (std::function<void()> *) ptr;

//     *tail+=1;
//     deque[*tail] = (quill_task *) malloc(sizeof(quill_task*));
//     deque[*tail]->work = (quill_task *) malloc(sizeof(quill_task));
//     memcpy((void*)deque[*tail]->work, (void *)ptr, sizeof(quill_task));
    
//     // std::function<void()> fun = *(deque[*tail]->work);
//     // fun();

//     printf("tail is: %d\n", *tail);

//     // deque[tail].work();
//     printf("phir bahar\n");
// }

void quill_runtime::push_task_to_runtime(quill_deque* pool, quill_task* ptr){
    // printf("pushing task \n");
    if (pool->head == -1) {
        //deque empty
        pool->head = 0;
    }

    // pool->head = pool->tail = -1;
    // pool->tail += 1;
    // pool->tasks = (quill_task **) malloc(sizeof(quill_task*));
    // pool->tasks[pool->tail] = (quill_task *) malloc(sizeof(quill_task));
// printf("pushing task \n");
    // memcpy((void *)&deque[++tail].work, ptr, sizeof(std::function<void()>));
    // deque[++tail].work = (std::function<void()> *) ptr;

    pool->tail += 1;
    // pool = (quill_deque *) malloc(sizeof(quill_task));
    // deque[0]->tasks = (quill_task **) malloc(50 * sizeof(quill_task*));
    // quill_task* (deque[0]->tasks)[deque[0]->tail] = (quill_task *) malloc(sizeof(quill_task));
    (pool->tasks)[pool->tail] = ptr;
    printf("pushing task \n");
    // printf("pu\n");
    // deque[0]->tail = (task_ds*) malloc(sizeof(task_ds));
    // deque[*tail]->work = (std::function<void()>*) malloc(sizeof(std::function<void()>));
    // memcpy((void*)deque[*tail]->work, (void *)ptr, sizeof(std::function<void()>));
    // quill_task fun = *((pool->tasks)[pool->tail]);
    // quill_task fun = *ptr;
    // fun();
    // std::function<void()> fun = *(deque[*tail]->work);
    // fun();

    // printf("tail is: %d\n", *tail);

    // deque[tail].work();
    printf("phir bahar\n");
}

quill_task* quill_runtime::grab_task_from_runtime(quill_deque* pool){
    //use locks for popping the task out from the deque
    quill_task* task;
    lock_finish();
    // printf("head: %d\n", head);
    if (pool->head <= pool->tail) {
        printf("head: %d tail: %d\n", pool->head, pool->tail);
        
        task = ((deque[0]->tasks)[pool->head++]);
        (*task)();
        // fun();

        // *head+=1;
        // return deque[*head-1]->work;
        // quill_task* fun = (std::function<void()>*) malloc(sizeof(std::function<void()>));
        // fun = (std::function<void()>*) malloc(sizeof(std::function<void()>));
        // memcpy((void*) fun, (void*)deque[*head]->work, sizeof(std::function<void()>));

        // std::function<void()> f = *fun;
        // f();
        // *head += 1;
        // lock_finish();
        // finish_counter--;
        // unlock_finish();
        // deque[head++].work();
        // head++;
        // l();
    } else task = NULL;
    unlock_finish();
    return task;
    // return NULL;
}
void quill::init_runtime() {
    // head=(int*)malloc(sizeof(int));
    // *head=-1;
    // tail=(int*)malloc(sizeof(int));
    // *tail=-1;

    printf("In init_runtime\n");
    deque = (quill_deque **) malloc(1 * sizeof(quill_deque*));
    deque[0] = (quill_deque *) malloc(sizeof(quill_deque));

    // int size = thread_pool_size();

    // thread = (pthread_t *) malloc (size*sizeof(pthread_t));

    // if (pthread_key_create(&ws_key, NULL) != 0) {
    //     perror("Cannot create ws_key for worker-specific data");
    // }
    
    // pthread_args *args = (pthread_args *) malloc(size * sizeof(pthread_args));
    //assigning id to the master thread
    // (&args[0])->id = 0;
    // pthread_mutex_init(&_lock, NULL);

    // for(int i=1; i<size; i++) {
    //     // ptr = (pthread_key_value *) malloc(sizeof(pthread_key_value));
    //     (&args[i])->id = i;
    //     // (void) pthread_setspecific(ws_key, (void *) new int(i));
    //     pthread_create(&thread[i], NULL, worker_routine, (void *)&args);
    // }
}

// void quill::init_runtime() {
//     head=(int*)malloc(sizeof(int));
//     *head=-1;
//     tail=(int*)malloc(sizeof(int));
//     *tail=-1;

//     printf("In init_runtime\n");
//     deque = (task_ds**) malloc(50 * sizeof(task_ds*));
//     int size = thread_pool_size();

//     thread = (pthread_t *) malloc (size*sizeof(pthread_t));

//     // if (pthread_key_create(&ws_key, NULL) != 0) {
//     //     perror("Cannot create ws_key for worker-specific data");
//     // }
    
//     // pthread_args *args = (pthread_args *) malloc(size * sizeof(pthread_args));
//     //assigning id to the master thread
//     // (&args[0])->id = 0;
//     pthread_mutex_init(&_lock, NULL);

//     // for(int i=1; i<size; i++) {
//     //     // ptr = (pthread_key_value *) malloc(sizeof(pthread_key_value));
//     //     (&args[i])->id = i;
//     //     // (void) pthread_setspecific(ws_key, (void *) new int(i));
//     //     pthread_create(&thread[i], NULL, worker_routine, (void *)&args);
//     // }
// }

void quill::start_finish(){
    printf("In start_finish\n");
    finish_counter = 0; //reset
}

void quill::async(std::function<void()> &&lambda) {
    printf("In async\n");
    lock_finish();
    finish_counter++; //concurrent access
    unlock_finish();
    // copy task on heap
    quill_task* p = (quill_task *) malloc(sizeof(quill_task));
    memcpy((void*)p, (void *)&lambda, sizeof(quill_task));


    //thread-safe push_task_to_runtime
    // if (*head == -1) {
    //     //deque empty
    //     *head = 0;
    // }
    // *tail+=1;
    // deque[*tail] = (task_ds*) malloc(sizeof(task_ds));
    // deque[*tail]->work = (std::function<void()>*) malloc(sizeof(std::function<void()>));
    // deque[*tail]->work = &lambda;
    
    // push_task_to_runtime();

    deque[0]->head = deque[0]->tail = -1;
    deque[0]->tasks = (quill_task **) malloc(sizeof(quill_task*));
    // deque[0]->tasks[0] = (quill_task *) malloc(sizeof(quill_task));

    push_task_to_runtime(deque[0], p);

    // std::function<void()>* x=(std::function<void()>*) p;
    // std::function<void()> fun=*x;
    // fun();

    
    // grab_task_from_runtime(deque[0]);
    // lambda();
    // printf("yes\n");
    return;
}

void quill::end_finish() {
    printf("In end_finish\n");
    while(finish_counter != 0) {
        grab_task_from_runtime(deque[0]);
        // find_and_execute_task();
        finish_counter = 0;
    }
}

void quill_runtime::execute_task(quill_task* task){
    printf("executing\n");
    (*task)();
    printf("ho kya rha h\n");
}

void quill_runtime::find_and_execute_task() {
    
    // grab_from_runtime is thread-safe
    quill_task* task = grab_task_from_runtime(deque[0]);
    if (task == NULL) printf("hmm\n");
    printf("aisa kya\n");
    if(task != NULL) {
        printf("start\n");
        // execute_task(task);
        std::function<void()> fun= *task;
        fun();
        printf("end\n");
        free((void*)task);
        lock_finish();
        finish_counter--;
        unlock_finish();
        printf("In find_and_execute %d\n", finish_counter);
    }
}

void quill::finalize_runtime() {
    //all spinning workers
    //will exit worker_routine
    shutdown = true;
    // int size = thread_pool_size();
    // // master waits for helpers to join
    // for(int i=1; i<size; i++) {
    //     pthread_join(thread[i], NULL);
    // }
    pthread_mutex_destroy(&_lock);
}