#include "stamp.h"
#include <iostream>
#include <pthread.h>
#include <cmath>

using namespace stamp;

struct thread_args1{
    std::function<void()> input_fun;
};

struct thread_args2{
    int no_of_times, offset, stride;
    std::function<void(int)> input_fun;
};

void *thread_func1(void *ptr){

    ((thread_args1 *) ptr)->input_fun();
    return NULL;
}

void *thread_func2(void *ptr){

    for (int i = 0; i < ((thread_args2 *) ptr)->no_of_times; i += ((thread_args2 *) ptr)->stride){
        ((thread_args2 *) ptr)->input_fun((((thread_args2 *) ptr)->offset) + i);
    }

    return NULL;
}

void stamp::execute_tuple(std::function<void()> &&lambda1, std::function<void()> &&lambda2){

    pthread_t thread;
    thread_args1 args;
    args.input_fun = lambda1;

    pthread_create(&thread, NULL, thread_func1, (void*) &args);
    lambda2();
    pthread_join(thread, NULL);
}

void stamp::parallel_for(int low, int high, int stride, std::function<void(int)> &&lambda, int numThreads){

    pthread_t *thread;
    thread_args2 *args;

    thread = (pthread_t *) malloc (numThreads*sizeof(pthread_t));
    args = (thread_args2 *) malloc (numThreads*sizeof(thread_args2));

    int t = 0;

    for (int i = 0; i < numThreads; i++){
        
        (&args[i])->no_of_times = (ceil)((double)(high-low)/(stride * (numThreads-t)));
        (&args[i])->input_fun = lambda;
        (&args[i])->offset = low;
        (&args[i])->stride = stride;
        low += stride * (&args[i])->no_of_times;
        pthread_create(&thread[i], NULL, thread_func2, (void*) &args[i]);
        // if ((void *)status != NULL){
        //     std::perror("Thread creation failed");
        //     exit(EXIT_FAILURE);
        // }
        t++;
        if (low >= high) break;
    }
    
    for (int i = 0; i < numThreads; i++){
        pthread_join(thread[i], NULL);
    }
}

void stamp::parallel_for(int high, std::function<void(int)> &&lambda, int numThreads){
    parallel_for(0, high, 1, (std::function<void(int)>)lambda, numThreads);
}