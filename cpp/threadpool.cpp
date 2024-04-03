#include "threadpool.hpp"
#include <iostream>


ThreadPool::ThreadPool(size_t threads) : stop(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back([this]
        {
            for(;;)
            {
                {
                    std::function<void()> task;
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock,[this]{ return this->stop || !this->tasks.empty(); });
                    if(this->stop && this->tasks.empty())
                        return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                    task();
                }
                
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    if (this->tasks.empty()) 
                    {
                        work_done.notify_all();
                    }
                }
                
            }
        });
}

void ThreadPool::enqueue( std::function<void()> F)
{
    std::unique_lock<std::mutex> lock(queue_mutex);

    if(stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");

    tasks.emplace(std::move(F));
    condition.notify_one();
}

void ThreadPool::wait_finish()
{
    std::unique_lock<std::mutex> lock(this->queue_mutex);
    if (!this->tasks.empty()) {
        this->work_done.wait(lock, [this] { return this->tasks.empty(); });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(auto & worker: workers)
    {
        worker.join();
    }
}

// https://www.cnblogs.com/sinkinben/p/16064857.html
// https://github.com/cbraley/threadpool/blob/master/src/thread_pool.h
// https://www.geeksforgeeks.org/thread-pool-in-cpp/