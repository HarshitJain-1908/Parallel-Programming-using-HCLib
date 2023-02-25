#include "quill-runtime.h"
#include "quill.h"
#include <pthread.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include "iostream"
#include <functional>

#define DEQUE_SIZE 400
using namespace quill_runtime;

struct pthread_args{
    int id;
};

volatile bool shutdown = false;
volatile int finish_counter = 0;
pthread_key_t ws_key;
pthread_t *thread;
pthread_mutex_t lock1, lock2;
quill_deque deque;

// void *quill_runtime::worker_routine(void *args){
//     printf("In worker_routine\n");
    
//     //assigns id to the thread
//     int id = ((pthread_args *)args)->id;
//     pthread_setspecific(ws_key, (void *)&id);

//     while (!shutdown){break;
//         // printf("hello bro\n");
//         // find_and_execute_task();
//     }
//     return NULL;
// }

// int quill_runtime::thread_pool_size(){
//     // int num_workers= atoi(getenv("QUILL_WORKERS"));
//     // return num_workers;
//     return 1;
// }

// void quill_runtime::lock_finish(){
//     pthread_mutex_lock(&_lock);
// }

// void quill_runtime::unlock_finish(){
//     pthread_mutex_unlock(&_lock);
// }

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

void quill_runtime::push_task_to_runtime(quill_task* ptr){

    if ((deque.tail == DEQUE_SIZE-1 && deque.head == 0) || (deque.tail == (deque.head-1)%(DEQUE_SIZE-1))){
        //deque overflow
        return;
    }else{
        if (deque.head == -1){
        deque.head = 0;
        deque.tail = 0;
        }else{
            if (deque.tail == DEQUE_SIZE-1 && deque.head != 0){
                deque.tail = 0;
            }else{
                deque.tail++;
            }
        }
    }
    
    printf("pushing task at %d \n", deque.tail);
    deque.tasks[deque.tail] = ptr;
    
}

quill_task* quill_runtime::grab_task_from_runtime(){
    //use locks for popping the task out from the deque
    quill_task* task = NULL;
    // lock_finish();

    pthread_mutex_lock(&lock2);
    if (deque.head == -1){
        return NULL;
    }else{
        task = deque.tasks[deque.head];
        if (deque.head == deque.tail){
            deque.head = -1;
            deque.tail = -1;
        }else{
            if (deque.head == DEQUE_SIZE-1){
                deque.head = 0;
            }else{
                deque.head++;
            }
        }
    }
    
    pthread_mutex_unlock(&lock2);
    return task;
    // return NULL;

    // printf("head: %d\n", head);
    // if (pool->head <= pool->tail) {
    //     printf("head: %d tail: %d\n", pool->head, pool->tail);
        
    //     // task = (pool->tasks)[pool->head++];
    //     // task = pool-
    //     // task = *(pool->tasks)[];
    //     task();
    //     // fun();

    //     // *head+=1;
    //     // return deque[*head-1]->work;
    //     // quill_task* fun = (std::function<void()>*) malloc(sizeof(std::function<void()>));
    //     // fun = (std::function<void()>*) malloc(sizeof(std::function<void()>));
    //     // memcpy((void*) fun, (void*)deque[*head]->work, sizeof(std::function<void()>));

    //     // std::function<void()> f = *fun;
    //     // f();
    //     // *head += 1;
    //     // lock_finish();
    //     // finish_counter--;
    //     // unlock_finish();
    //     // deque[head++].work();
    //     // head++;
    //     // l();
    // } else task = NULL;
    // unlock_finish();
    // return task;
    // return NULL;
}

void quill::init_runtime() {
    // head=(int*)malloc(sizeof(int));
    // *head=-1;
    // tail=(int*)malloc(sizeof(int));
    // *tail=-1;

    printf("In init_runtime\n");
    // deque.task = malloc(sizeof(quill_task));

    deque.head = -1;
    deque.tail = -1;
    

    // deque = (quill_deque **) malloc(1 * sizeof(quill_deque*));--
    // deque[0] = (quill_deque *) malloc(sizeof(quill_deque));--

    // int size = thread_pool_size();

    // thread = (pthread_t *) malloc (size*sizeof(pthread_t));

    // if (pthread_key_create(&ws_key, NULL) != 0) {
    //     perror("Cannot create ws_key for worker-specific data");
    // }
    
    // pthread_args *args = (pthread_args *) malloc(size * sizeof(pthread_args));
    //assigning id to the master thread
    // (&args[0])->id = 0;
    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);
    deque.tasks = new quill_task*[DEQUE_SIZE];

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
    // pthread_mutex_init(&_lock, NULL);

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
    // lock_finish();
    pthread_mutex_lock(&lock1);
    finish_counter++; //concurrent access
    // unlock_finish();
    pthread_mutex_unlock(&lock1);
    // copy task on heap
    // quill_task* p = (quill_task *) malloc(sizeof(quill_task));
    // quill_task *p = (quill_task *) malloc(sizeof(quill_task));
    // memcpy((void *)p, (void *)&lambda, sizeof(quill_task));
    // deque[0]->head = deque[0]->tail = -1;
    // deque[0]->tasks = (quill_task *) malloc(sizeof(quill_task));
    // deque[0]->tasks[0] = (quill_task *) malloc(sizeof(quill_task));

    // deque.head = deque.tail = -1;
    // deque.task = *(new quill_task(lambda));
    quill_task* p = new quill_task(lambda);
    if (deque.head == -1){
        deque.head = 0;
    }
    
    deque.tail += 1;
    printf("pushing task at %d \n", deque.tail);

    // std::function<void()>* p1 = new std::function<void()> (ptr);
    // deque.tasks
    // deque.tasks[deque.tail] = p;
    push_task_to_runtime(p);

    // quill_runtime::push_task_to_runtime(deque, *p);

    // deque[0]->tasks = (quill_task *) malloc(sizeof(quill_task));
    // quill_runtime::push_task_to_runtime(deque[0], *p);--

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

    // grab_task_from_runtime(deque[0]);
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
        // (*(((deque[0])->tasks)[0]))();
        // if (deque[0]->tasks[0] != NULL){
        printf("hello\n");
        printf("head: %d\n", deque.head);
        // grab_task_from_runtime(deque[0]);
        // grab_task_from_runtime();
        // }
        find_and_execute_task();
        // finish_counter = 0;
    }
}


void quill_runtime::find_and_execute_task() {
    
    // grab_from_runtime is thread-safe

    quill_task* task = grab_task_from_runtime();
    // std::function<void()>* t = (std::function<void()>*) malloc(sizeof(std::function<void()>));
    // *t = task;
    if (task == NULL) printf("hmm\n");
    else{
        
        // execute_task(task);
        std::function<void()> fun=*task;
        fun();
        std::cout<<"completed"<<std::endl;
        // free(task);
        // // (*t)();
        // delete()
        // free((void*)t);
        // free
        // lock_finish();
        pthread_mutex_lock(&lock1);
        finish_counter--;
        pthread_mutex_unlock(&lock1);

    }
    // printf("aisa kya\n");
    // if(task != NULL) {
    //     printf("start\n");
    //     // execute_task(task);
    //     // std::function<void()> fun= *task;
    //     // fun();
    //     printf("end\n");
        // free((void*)task);
    //     lock_finish();
    //     finish_counter--;
    //     unlock_finish();
    //     printf("In find_and_execute %d\n", finish_counter);
    // }
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
    // pthread_mutex_destroy(&lock1);
    // pthread_mutex_destroy(&lock2);
}