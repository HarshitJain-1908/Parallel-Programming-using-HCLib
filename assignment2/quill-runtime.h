#include <functional>

#define quill_task std::function<void()>

namespace quill_runtime{

    typedef struct{
        quill_task **deque; //deque[0]=(quill_task*)malloc(sizeof(quill_task));     // deque[0]=> of type *quill_task
        int head, tail, idx;
        // int idx;
    }quill_deque;

    void *worker_routine(void *args);
    int thread_pool_size();
    void find_and_execute_task();
    void push_task_to_runtime(std::function<void()>* ptr);
    std::function<void()>* grab_task_from_runtime();
    void lock_finish();
    void unlock_finish();
    void execute_task(std::function<void()>* lambda);
    void find_and_execute_task();
}