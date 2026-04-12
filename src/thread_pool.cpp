// thread_pool.cpp — COMPLETE FIXED FILE
#include "thread_pool.h"

ThreadPool::ThreadPool(size_t num_threads) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    condition.wait(lock, [this] {
                        return stop || !tasks.empty();
                    });
                    if (stop && tasks.empty()) return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task(); // run the job

                // Mark one task as done, notify wait_all()
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    --active_tasks;
                }
                finished_condition.notify_all();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        ++active_tasks; // count this new task
        tasks.emplace(std::move(task));
    }
    condition.notify_one();
}

// Blocks the calling thread until active_tasks == 0
// i.e. every enqueued task has fully completed
void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    finished_condition.wait(lock, [this] {
        return active_tasks == 0 && tasks.empty();
    });
}










