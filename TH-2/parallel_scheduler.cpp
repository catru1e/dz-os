#include "parallel_scheduler.hpp"

parallel_scheduler::parallel_scheduler(size_t capacity) {
    workers.reserve(capacity);
    for (size_t i = 0; i < capacity; i++) {
        workers.emplace_back(&parallel_scheduler::worker_loop, this);
    }
}

parallel_scheduler::~parallel_scheduler() {
    stop = true;
    cv.notify_all();

    for (auto &t : workers)
        if (t.joinable()) t.join();
}

void parallel_scheduler::worker_loop() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [&]() {
                return stop || !tasks.empty();
            });

            if (stop && tasks.empty())
                return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        task();
    }
}
