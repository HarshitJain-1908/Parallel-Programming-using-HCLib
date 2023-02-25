#include "quill-runtime.h"
#include "quill.h"
#include <pthread.h>
#include <cstring>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include "iostream"
#include <functional>

using namespace quill_runtime;

struct pthread_args{
    // int id;
    quill_deque* deq;
};

volatile bool shutdown = false;
volatile int finish_counter = 0;
pthread_key_t ws_key;
pthread_t *thread;
pthread_mutex_t lock1;
quill_deque *deque;

void *quill_runtime::worker_routine(void *args){
    
    //assigns id to the thread
    // int id = ((pthread_args *)args)->id;
    quill_deque* pool = (*((pthread_args *) args)).deq;
    pthread_mutex_init(&((*pool).lock), NULL);
    pthread_setspecific(ws_key, (void *)pool);

    while (!shutdown){
        printf("In worker_routine\n");
        find_and_execute_task();
    }
    return NULL;
}

int quill_runtime::thread_pool_size(){
    // int num_workers= atoi(getenv("QUILL_WORKERS"));
    // return num_workers;
    return 2;
}

void quill_runtime::lock_finish(){
    pthread_mutex_lock(&lock1);
}

void quill_runtime::unlock_finish(){
    pthread_mutex_unlock(&lock1);
}

void quill_runtime::lock_deque(quill_deque* deq){
    pthread_mutex_lock(&((*deq).lock));
}

void quill_runtime::unlock_deque(quill_deque* deq){
    pthread_mutex_unlock(&((*deq).lock));
}


void quill_runtime::push_task_to_runtime(quill_deque* deq, quill_task* ptr){

    if (((*deq).tail == DEQUE_SIZE-1 && (*deq).head == 0) || ((*deq).tail == ((*deq).head-1)%(DEQUE_SIZE-1))){
        //deque overflow
        return;
    }else{
        if ((*deq).head == -1){
            // printf("me too\n");
            (*deq).head = 0;
            (*deq).tail = 0;
        }else{
            if ((*deq).tail == DEQUE_SIZE-1 && (*deq).head != 0){
                (*deq).tail = 0;
            }else{
                (*deq).tail++;
            }
        }
    }
    
    printf("pushing task, tail: %d head is: %d\n", (*deq).tail, (*deq).head);
    (*deq).tasks[(*deq).tail] = *ptr;
}

quill_task quill_runtime::grab_task_from_runtime(quill_deque* deq){
    quill_task task;
    task = pop(deq);
    if (task != NULL){
        return task;
    }else{
        //victim becomes thief
        int steal_deque = rand() % thread_pool_size();
        printf("stealing from deque: %d", steal_deque);
        task = steal(&deque[steal_deque]);
        if (task != NULL){
            return task;
        }
    }
    return NULL;
}

quill_task quill_runtime::pop(quill_deque* deq){
    //use locks for popping the task out from the deque
    quill_task task = NULL;

    lock_deque(deq);
    if (deq == NULL) printf("aisa h kya\n");
    if ((*deq).tail == -1){
        printf("I am here.\n");
        return NULL;
    }else{
        printf("pop task from %d\n", (*deq).tail);
        task = (*deq).tasks[(*deq).tail];
        if ((*deq).head == (*deq).tail){
            (*deq).head = -1;
            (*deq).tail = -1;
        }else{
            (*deq).tail--;
        }
    }
    
    unlock_deque(deq);
    return task;
}

quill_task quill_runtime::steal(quill_deque* deq){
    //use locks for popping the task out from the deque
    quill_task task = NULL;

    lock_deque(deq);
    if ((*deq).head == -1){
        printf("I am here.\n");
        return NULL;
    }else{
        printf("steal task from head: %d\n", (*deq).head);
        task = (*deq).tasks[(*deq).head];
        if ((*deq).head == (*deq).tail){
            (*deq).head = -1;
            (*deq).tail = -1;
        }else{
            if ((*deq).head == DEQUE_SIZE-1){
                (*deq).head = 0;
            }else{
                (*deq).head++;
            }
        }
    }
    
    unlock_deque(deq);
    return task;
}
void quill::init_runtime() {

    printf("In init_runtime\n");

    int size = thread_pool_size();
   
    deque = (quill_deque*) malloc(size*sizeof(quill_deque));
    
    //intialising deque of all the workers
    for (int i = 0; i < size; i++){
        // deque[i] = (quill_deque*) malloc(sizeof(quill_deque));
        // deque[i] = (quill_deque) malloc(sizeof(quill_deque));
        deque[i].head = -1;
        deque[i].tail = -1;
        deque[i].tasks = (quill_task*) malloc(DEQUE_SIZE*sizeof(quill_task));
        // deque[i].tasks = new quill_task*[DEQUE_SIZE];
    }
    // printf("pushing task at %d head is: %d\n", deque[0].tail, deque[0].head);
    thread = (pthread_t *) malloc (size*sizeof(pthread_t));

    if (pthread_key_create(&ws_key, NULL) != 0) {
        perror("Cannot create ws_key for worker-specific data");
    }
    
    pthread_args *args = (pthread_args *) malloc(size * sizeof(pthread_args));
    //assigning id to the master thread
    // (&args[0])->id = 0;
    (&args[0])->deq = &deque[0];
    pthread_setspecific(ws_key, (void *)(&deque[0]));

    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&(deque[0].lock), NULL);

    for(int i=1; i<size; i++) {
        (&args[i])->deq = &deque[i];
        pthread_create(&thread[i], NULL, worker_routine, (void *)(&args[i]));
    }
}


void quill::start_finish(){
    printf("In start_finish\n");
    finish_counter = 0; //reset
}

void quill::async(std::function<void()> &&lambda) {
    // printf("In async\n");
    lock_finish();
    finish_counter++; //concurrent access
    unlock_finish();

    // copy task on heap
    quill_task* p = new quill_task(lambda);
    // int* idx = (int*) pthread_getspecific(ws_key);
    // pthread_args
    quill_deque* deq = (quill_deque*) pthread_getspecific(ws_key);
    // quill_deque* deq = (quill_deque*) (*(pthread_args *)(*((pthread_args**) pthread_getspecific(ws_key)))).deq;
    // printf("idx: %d\n", *idx);

    // printf("idx: %d\n", idx);
    printf("calling push\n");
    push_task_to_runtime(deq, p);
    return;
}

void quill::end_finish() {
    printf("In end_finish\n");
    while(finish_counter != 0) {
        // printf("head: %d\n", deque.head);
        find_and_execute_task();
    }
}

void quill_runtime::execute_task(quill_task task){
    task();
    std::cout<<"completed"<<std::endl;
}

void quill_runtime::find_and_execute_task() {
    
    // grab_from_runtime is thread-safe
    // int idx = ((pthread_args*) pthread_getspecific(ws_key))->id;
    quill_deque* deq =  (quill_deque*) pthread_getspecific(ws_key);
    // printf("trying to grab idx: %d\n", idx);

    quill_task task = grab_task_from_runtime(deq);
    printf("doing smthg\n");
    if (task == NULL) {
        printf("null mila\n");
        // return;
    }
    else{
        execute_task(task);        
        // free((void *)task);
        pthread_mutex_lock(&lock1);
        finish_counter--;
        pthread_mutex_unlock(&lock1);

    }
    // task = deque[0].tasks[0];
    // if (task != NULL) {
    //     printf("I got a task from %d.\n", deque[0].head);
    //     task();
    // }
    // else printf("Nope\n");
    return;
}

void quill::finalize_runtime() {
    //all spinning workers
    //will exit worker_routine
    shutdown = true;
    printf("shutting down\n");
    int size = thread_pool_size();
    // master waits for helpers to join
    for(int i=1; i<size; i++) {
        pthread_join(thread[i], NULL);
    }
    pthread_mutex_destroy(&lock1);
    for (int i = 0; i < size; i++){
        quill_deque d = deque[i];
        pthread_mutex_destroy(&(d.lock));
    }
}