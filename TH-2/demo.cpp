#include "parallel_scheduler.hpp"
#include <iostream>
#include <chrono>
#include <thread>

void slow_task(int id) {
    std::cout << "Task " << id << " started\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Task " << id << " finished\n";
}

int main() {
    parallel_scheduler scheduler(3); // pool with 3 threads

    // Enqueue more tasks than capacity
    for (int i = 1; i <= 10; i++) {
        scheduler.run(slow_task, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(6));
    std::cout << "Demo finished\n";
    return 0;
}
