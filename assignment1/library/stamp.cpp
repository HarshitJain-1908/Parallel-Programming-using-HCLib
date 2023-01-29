#include "stamp.h"
#include <iostream>
#include <pthread.h>
#include <cmath>

struct thread_args1{
    std::function<void()> input_fun;
};

void *thread_func1(void *ptr){

    ((thread_args1 *) ptr)->input_fun();
    return NULL;
}

void execute_tuple(std::function<void()> &&lambda1, std::function<void()> &&lambda2){

    pthread_t thread;
    thread_args1 args;
    args.input_fun = lambda1;

    // time_t start1, end1;
    // executionTime(start1);

    pthread_create(&thread, NULL, thread_func1, (void*) &args);
    lambda2();
    pthread_join(thread, NULL);

    // executionTime(end1);
    // double time_taken = double(end1 - start1);
    // cout << "Time taken for execution is : " << fixed << time_taken << setprecision(5) << " sec " << endl;
}