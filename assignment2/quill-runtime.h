#include <functional>

#define quill_task std::function<void()>

namespace quill_runtime{

    typedef struct{
        std::function<void()>** tasks; //deque[0]=(quill_task*)malloc(sizeof(quill_task));     // deque[0]=> of type quill_task*
        int head, tail, idx;
    }quill_deque;

    // void *worker_routine(void *args);
    // int thread_pool_size();
    void push_task_to_runtime(quill_task* ptr);
    quill_task* grab_task_from_runtime();
    // void lock_finish();
    // void unlock_finish();
    // void execute_task(quill_task* task);
    void find_and_execute_task();
}