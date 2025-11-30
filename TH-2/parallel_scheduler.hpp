#ifndef PARALLEL_SCHEDULER_HPP
#define PARALLEL_SCHEDULER_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <atomic>

class parallel_scheduler {
public:
    explicit parallel_scheduler(size_t capacity);
    ~parallel_scheduler();

    // function takes func(arg)
    template <typename Func, typename Arg>
    void run(Func f, Arg arg) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.push([=]() { f(arg); });
        }
        cv.notify_one();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> stop{false};

    void worker_loop();
};

#endif
