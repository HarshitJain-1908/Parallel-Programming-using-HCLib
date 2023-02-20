#include <functional>

namespace quill{

    void init_runtime();
    void start_finish();
    void async(std::function<void()> &&lambda); //accepts a C++11 lambda function
    void end_finish();
    void finalize_runtime();

}