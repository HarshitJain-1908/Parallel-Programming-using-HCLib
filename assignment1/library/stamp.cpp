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

struct thread_args3{
    int no_of_times, low, high, size, outer, inner, offset1, offset2, stride1, stride2;
    std::function<void(int, int)> input_fun;
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

void *thread_func3(void *ptr){

        
        for (int i = ((thread_args3 *) ptr)->low; i < ((thread_args3 *) ptr)->high; i++){
            ((thread_args3 *) ptr)->input_fun(i/((thread_args3 *) ptr)->size, i%((thread_args3 *) ptr)->size);
        }
        // int count = 0;
        // printf("stride1 %d\n", ((thread_args3 *) ptr)->stride1);
        // printf("stride2 %d\n", ((thread_args3 *) ptr)->stride2);
        // for (int i = 0; i <= ((thread_args3 *) ptr)->outer; i += ((thread_args3 *) ptr)->stride1){
            
        //     for (int j = 0; j < ((thread_args3 *) ptr)->inner; j += ((thread_args3 *) ptr)->stride2){
        //         // printf("ok %d %d\n", count, j);
        //         printf("\ni: %d j : %d\n", (((thread_args3 *) ptr)->offset1) + i, (((thread_args3 *) ptr)->offset2) + j);
        //         ((thread_args3 *) ptr)->input_fun((((thread_args3 *) ptr)->offset1) + i, (((thread_args3 *) ptr)->offset2) + j);
        //         count++;
        //         if (count == ((thread_args3 *) ptr)->no_of_times) break;
        //     }
        //     if (count == ((thread_args3 *) ptr)->no_of_times) break;
        // }
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

void stamp::parallel_for(int low1, int high1, int stride1, int low2, int high2, int stride2,
    std::function<void(int, int)> &&lambda, int numThreads){

    pthread_t *thread;
    thread_args3 *args;

    thread = (pthread_t *) malloc (numThreads*sizeof(pthread_t));
    args = (thread_args3 *) malloc (numThreads*sizeof(thread_args3));

    int t = 0, offset = 0;
    int total_tasks = (ceil)((double)((high1-low1)*(high2-low2)/(stride1*stride2)));
    for (int i = 0; i < numThreads; i++){
        
        (&args[i])->no_of_times = (ceil)((double)(total_tasks - offset)/(numThreads-t));
        (&args[i])->low = offset;
        (&args[i])->input_fun = lambda;
        (&args[i])->high = (&args[i])->low + (&args[i])->no_of_times;
        (&args[i])->size = high1 - low1;
        offset += (&args[i])->no_of_times;
        printf("no_of_times %d low %d high %d offset %d\n", (&args[i])->no_of_times, (&args[i])->low, (&args[i])->high, offset);

        pthread_create(&thread[i], NULL, thread_func3, (void*) &args[i]);
        t++;
        if (offset >= total_tasks) break;
    }
    printf("\n");
    
    for (int i = 0; i < numThreads; i++){
        pthread_join(thread[i], NULL);
    }
}