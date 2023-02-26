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

volatile bool shutdown = false;
volatile int finish_counter = 0;
pthread_key_t ws_key;
pthread_t *thread;
pthread_mutex_t lock1;
quill_deque *deque;

void *quill_runtime::worker_routine(void *args){
    
    //assigns id to the thread
    int i = *((int *)args);

    quill_deque* pool = (quill_deque*) (&deque[i]);

    pthread_setspecific(ws_key, (void *)(pool));

    while (!shutdown){
        find_and_execute_task();
    }
    return NULL;
}

int quill_runtime::thread_pool_size(){
    char *val = getenv("QUILL_WORKERS");
    int num_workers = val != NULL ? std::__cxx11::stoi(val) : 1;
    return num_workers;
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
        printf("Oops! Deque Overflow, increase deque size\n");
        exit(1);
    }else{
        if ((*deq).head == -1){
            (*deq).tail = 0;
            (*deq).tasks[(*deq).tail] = *ptr;
            (*deq).head = 0;
        }else{
            if ((*deq).tail == DEQUE_SIZE-1 && (*deq).head != 0){
                (*deq).tail = 0;
            }else{
                (*deq).tail++;
            }
            (*deq).tasks[(*deq).tail] = *ptr;
        }
    }
}

quill_task quill_runtime::grab_task_from_runtime(quill_deque* deq){

    quill_task task;
    int cur_id;
    

    cur_id = (*deq).id;
    task = pop(deq);

    if (task != NULL){
        return task;
    }else{
        //victim becomes thief
        
        int s = thread_pool_size();
        int r = rand() % s;
        while (cur_id == r){
            r = rand() % s;
            continue;
        }

        task = steal(&deque[r], cur_id);
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

    if ((*deq).tail != -1){
        task = (*deq).tasks[(*deq).tail];
        if ((*deq).head == (*deq).tail){
            (*deq).tail = -1;
            (*deq).head = -1;
        }else{
            (*deq).tail--;
        }
    }
    unlock_deque(deq);
    return task;
}

quill_task quill_runtime::steal(quill_deque* deq, int cur_id){

    quill_task task = NULL;
    //use locks for popping the task out from the deque
    lock_deque(deq);
    
    if ((*deq).head != -1){
        task = (*deq).tasks[(*deq).head];
        
        if ((*deq).head == (*deq).tail){
            (*deq).tail = -1;
            (*deq).head = -1;
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

    pthread_mutex_init(&lock1, NULL);
    int size = thread_pool_size();

    deque = (quill_deque*) malloc(size*sizeof(quill_deque));
    
    //intialising deque of all the workers
    for (int i = 0; i < size; i++){
        deque[i].head = -1;
        deque[i].tail = -1;
        deque[i].id = i;
        deque[i].tasks = (quill_task*) malloc(DEQUE_SIZE*sizeof(quill_task));
        pthread_mutex_init(&(deque[i].lock), NULL);
    }
    thread = (pthread_t *) malloc (size*sizeof(pthread_t));

    if (pthread_key_create(&ws_key, NULL) != 0) {
        perror("Cannot create ws_key for worker-specific data");
    }
    
    pthread_setspecific(ws_key, (void *)(&deque[0]));

    for(int i=1; i<size; i++) {
        pthread_create(&thread[i], NULL, worker_routine, (void *)(new int(i)));
    }
}


void quill::start_finish(){
    finish_counter = 0; //reset
}

void quill::async(std::function<void()> &&lambda) {
    lock_finish();
    finish_counter++; //concurrent access
    unlock_finish();

    // copy task on heap
    quill_task* p = new quill_task(lambda);
    quill_deque* deq = (quill_deque*) pthread_getspecific(ws_key);
    push_task_to_runtime(deq, p);
    return;
}

void quill::end_finish() {
    while(finish_counter != 0) {
        find_and_execute_task();
    }

    int size = thread_pool_size();
    shutdown = true;
    // master waits for helpers to join
    for(int i=1; i<size; i++) {
        pthread_join(thread[i], NULL);
    }
}

void quill_runtime::execute_task(quill_task task){
    task();
}

void quill_runtime::find_and_execute_task() {

    // grab_from_runtime is thread-safe

    quill_deque* deq = (quill_deque*) pthread_getspecific(ws_key);
    quill_task task = grab_task_from_runtime(deq);

    if (task != NULL) {
        execute_task(task);
        lock_finish();
        finish_counter--;
        unlock_finish();
    }
    return;
}

void quill::finalize_runtime() {

    //all spinning workers
    //will exit worker_routine
    
    int size = thread_pool_size();

    pthread_mutex_destroy(&lock1);
    for (int i = 0; i < size; i++){
        pthread_mutex_destroy(&(deque[i].lock));
    }
}