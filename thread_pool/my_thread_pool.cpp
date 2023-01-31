#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <queue>
#include <functional>

class task
{
public:
    task(int priority, std::function<void()> fun);
    int priority;
    std::function<void()> fun;
};

task::task(int priority = 1, std::function<void()> fun): priority(priority), fun(fun) {} // priority越大则优先级越高

class threadPool
{
public:
    threadPool(int minPoolSize = 10); // 默认10个线程
    ~threadPool();
    void Start();
    void Stop();

private:
    struct taskCmp
    {
        bool operator()(const task *t1, const task *t2)
        {
            return t1->priority < t2->priority;
        }
    };

private:
    std::vector<std::thread*> pool;
    int minPoolSize;
    bool isRunning;
    std::mutex lockMutex;
    std::condition_variable lockCondition;
    std::priority_queue<task*, std::vector<task*>, taskCmp> taskQue;
};

threadPool::threadPool(int minPoolSize)
{
    if (minPoolSize <= 0) minPoolSize = 10;
    this->minPoolSize = minPoolSize;
    this->pool.resize(this->minPoolSize);
}

threadPool::~threadPool()
{
    std::lock_guard<std::mutex> lock(this->lockMutex);
    
}