#include <functional>

#define quill_task std::function<void()>
#define DEQUE_SIZE 50

namespace quill_runtime{

    typedef struct{
        quill_task *tasks;
        int head, tail, id;
        pthread_mutex_t lock;
    }quill_deque;

    void *worker_routine(void *args);
    int thread_pool_size();
    void push_task_to_runtime(quill_deque* deq, quill_task* ptr);
    quill_task grab_task_from_runtime(quill_deque* deq);
    quill_task pop(quill_deque* deq);
    quill_task steal(quill_deque* deq, int cur_id);
    void lock_finish();
    void unlock_finish();
    void lock_deque(quill_deque* deq);
    void unlock_deque(quill_deque* deq);
    void execute_task(quill_task task);
    void find_and_execute_task();
}