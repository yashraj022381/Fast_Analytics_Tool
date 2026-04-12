// thread_pool.h — COMPLETE FIXED FILE
#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t num_threads = 4);
    ~ThreadPool();

    void enqueue(std::function<void()> task);

    // NEW: blocks until every queued task has finished
    // Call this in main() before reading results!
    void wait_all();

private:
    std::vector<std::thread>          workers;
    std::queue<std::function<void()>> tasks;
    std::mutex                        queue_mutex;
    std::condition_variable           condition;
    std::condition_variable           finished_condition; // NEW
    std::atomic<int>                  active_tasks{0};    // NEW
    bool                              stop = false;
};












