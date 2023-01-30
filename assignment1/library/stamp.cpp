#include "stamp.h"
#include <iostream>
#include <pthread.h>
#include <cmath>
#include <time.h>

using namespace stamp;

//structure for passing arguments to pthread_create with required attributes for API used in fibonacci
struct thread_args1{
    std::function<void()> input_fun;
};

//structure for passing arguments to pthread_create with required attributes for API used in vector addition
struct thread_args2{
    int no_of_times, offset, stride;
    std::function<void(int)> input_fun;
};

//structure for passing arguments to pthread_create with required attributes for API used in matrix multiplication
struct thread_args3{
    int low, high, size;
    std::function<void(int, int)> input_fun;
};

//function to be executed by thread to perform the task assigned in fibonacci program
void *thread_func1(void *ptr){

    ((thread_args1 *) ptr)->input_fun();
    return NULL;
}

//function to be executed by thread to perform the task assigned in vector addition program
void *thread_func2(void *ptr){

    for (int i = 0; i < ((thread_args2 *) ptr)->no_of_times; i++){
        ((thread_args2 *) ptr)->input_fun((((thread_args2 *) ptr)->offset) + i*((thread_args2 *) ptr)->stride);
    }
    return NULL;
}

//function to be executed by thread to perform the task assigned in matrix multiplication program
void *thread_func3(void *ptr){
        
    for (int i = ((thread_args3 *) ptr)->low; i < ((thread_args3 *) ptr)->high; i++){
        ((thread_args3 *) ptr)->input_fun(i/((thread_args3 *) ptr)->size, i%((thread_args3 *) ptr)->size);
    }
    return NULL;
}

//multithreaded API to do fibonacci calculation
void stamp::execute_tuple(std::function<void()> &&lambda1, std::function<void()> &&lambda2){

    clock_t start, end;
    double time_taken;
    start = clock();

    int numThreads = 1;
    pthread_t thread;
    thread_args1 args;
    //mapping tasks to the thread
    args.input_fun = lambda1;
    
    int status;
    status = pthread_create(&thread, NULL, thread_func1, (void*) &args); //creating thread for executing lambda1 in parallel
    if (status != 0){
        perror("pthread_create() error");
        exit(1);
    }
    lambda2(); //executing this in the same thread as no other work to do in this thread
    status = pthread_join(thread, NULL);
    if (status != 0){
        perror("pthread_join() error");
        exit(1);
    }
    end = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    std::cout << "StaMp Statistics: " << numThreads << ", Parallel execution time = " << time_taken << " sec " <<std::endl;
}

//multithreaded API to do vector addition
void stamp::parallel_for(int low, int high, int stride, std::function<void(int)> &&lambda, int numThreads){

    clock_t start, end;
    double time_taken;
    start = clock();

    pthread_t *thread;
    thread_args2 *args;

    thread = (pthread_t *) malloc (numThreads*sizeof(pthread_t));
    args = (thread_args2 *) malloc (numThreads*sizeof(thread_args2));

    int t = 0, status;

    for (int i = 0; i < numThreads; i++){
        
        //mapping a number of tasks to each of the threads
        (&args[i])->no_of_times = (ceil)((double)(high-low)/(stride * (numThreads-t)));
        (&args[i])->input_fun = lambda;
        (&args[i])->offset = low;
        (&args[i])->stride = stride;
        low += stride * (&args[i])->no_of_times;
        
        status = pthread_create(&thread[i], NULL, thread_func2, (void*) &args[i]);
        if (status != 0){
            perror("pthread_create() error");
            exit(1);
        }
        t++;
        if (low >= high) break;
    }
    
    for (int i = 0; i < numThreads; i++){
        status = pthread_join(thread[i], NULL);
        if (status != 0){
            perror("pthread_join() error");
            exit(1);
        }
    }
    end = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    std::cout << "StaMp Statistics: " << numThreads << ", Parallel execution time = " << time_taken << " sec " <<std::endl;
}

//multithreaded API to do vector addition
void stamp::parallel_for(int high, std::function<void(int)> &&lambda, int numThreads){
    //calling above defined function with required parameters to avoid code repetition
    //providing pre-defined values to suitable parameters and type casting lambda function before passing
    stamp::parallel_for(0, high, 1, (std::function<void(int)>)lambda, numThreads);
}

//multithreaded API to do matrix multiplication
void stamp::parallel_for(int low1, int high1, int stride1, int low2, int high2, int stride2,
    std::function<void(int, int)> &&lambda, int numThreads){
    
    clock_t start, end;
    double time_taken;
    start = clock();

    pthread_t *thread;
    thread_args3 *args;

    thread = (pthread_t *) malloc (numThreads*sizeof(pthread_t));
    args = (thread_args3 *) malloc (numThreads*sizeof(thread_args3));

    int status, t = 0, offset = 0, n;
    int total_tasks = (ceil)((double)((high1-low1)*(high2-low2)/(stride1*stride2)));
    for (int i = 0; i < numThreads; i++){
        
        //mapping a number of tasks to each of the threads
        n = (ceil)((double)(total_tasks - offset)/(numThreads-t));
        (&args[i])->low = offset;
        (&args[i])->input_fun = lambda;
        (&args[i])->high = (&args[i])->low + n;
        (&args[i])->size = high1 - low1;
        offset += n;
        status = pthread_create(&thread[i], NULL, thread_func3, (void*) &args[i]);
        if (status != 0){
            perror("pthread_create() error");
            exit(1);
        }
        t++;
        if (offset >= total_tasks) break;
    }
    
    for (int i = 0; i < numThreads; i++){
        status = pthread_join(thread[i], NULL);
        if (status != 0){
            perror("pthread_join() error");
            exit(1);
        }
    }
    end = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    std::cout << "StaMp Statistics: " << numThreads << ", Parallel execution time = " << time_taken << " sec " <<std::endl;
}

//multithreaded API to do matrix multiplication
void parallel_for(int high1, int high2, std::function<void(int, int)> &&lambda, int numThreads){
    //calling above defined function with required parameters to avoid code repetition
    //providing pre-defined values to suitable parameters and type casting lambda function before passing
    stamp::parallel_for(0, high1, 1, 0, high2, 1, (std::function<void(int, int)>) lambda, numThreads);
}