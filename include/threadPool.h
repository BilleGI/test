#pragma once
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template<class F, class... Args>
    auto commit(F&&, Args&& ...) ->std::future<decltype(std::declval<F>()(std::declval<Args>()...))>;
private:
    using Task = std::function<void()>;
    std::vector<std::thread> wokers;
    std::queue<Task> taskList;
    std::mutex mtx;
    std::atomic_bool done;
    std::condition_variable cv;
};

ThreadPool::ThreadPool() : done{false}
{
    size_t allPull{std::thread::hardware_concurrency()};
    for(size_t i{}; i < allPull; ++i)
    {
        wokers.emplace_back([this]
                            {
                                while(!this->done)
                                {
                                    Task task;
                                    {
                                        std::unique_lock<std::mutex> lock{mtx};
                                        cv.wait(lock, [this]{return done.load() || !taskList.empty();});
                                        if(done || taskList.empty()) return;
                                        task = std::move(taskList.front());
                                        taskList.pop();
                                    }
                                    task();
                                }
                            });
    }
}

ThreadPool::~ThreadPool()
{
    done.store(true);
    cv.notify_all();
    for(auto& woker: wokers)
        if(woker.joinable()) woker.join();
}

template <class F, class... Args>
auto ThreadPool::commit(F&& f, Args&& ... args)->std::future<decltype(std::declval<F>()(std::declval<Args>()...))>
{
    if(done.load())
    {
        throw std::runtime_error("Thread pool has been closed");
    }

    using retType = decltype(f(args...));
    auto task = std::make_shared<std::packaged_task<retType ()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<retType> future = task->get_future();

    {
        std::lock_guard<std::mutex> lock{mtx};
        taskList.emplace([task]
                         {
                             (*task)();
                         });
    }
    cv.notify_one();

    return future;
}
