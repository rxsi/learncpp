#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <queue>
#include <functional>

#define _MIN_THREAD_NUM 5 // 最少5个线程
#define _MAX_THREAD_NUM 100 // 最多100个线程
#define _DEFAULT_PRIORITY 10 // 默认优先级是10

struct Task
{
    int priority; // 优先级，值越小代表优先级越高
    void (*func)(); // 函数指针，会用std::function 和 std::bind 封装
    Task(int _priority, void (*_func)()): priority(_priority), func(_func){}
};

class ThreadPool
{
public:
    void Start(int);
    void Stop();
    void AddTask(Task*);

    ThreadPool();
    ~ThreadPool();
private:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    void ThreadLoop();
    
private:
    struct _TaskCmp
    {
        bool operator() (const Task* t1, const Task* t2)
        {
            return t1->priority > t2->priority;
        }
    };

private:
    bool isRunning = false;
    std::mutex _myMutex;
    std::condition_variable _myCon;
    std::vector<std::thread*> threads;
    std::priority_queue<Task*, std::vector<Task*>, _TaskCmp> tasks;
};

void ThreadPool::Start(int threadNum = _MIN_THREAD_NUM)
{
    if (isRunning)
    {
        std::cout << "thread pool is running" << std::endl;
        return;
    }
    if (threadNum < _MIN_THREAD_NUM) threadNum = _MIN_THREAD_NUM;
    if (threadNum > _MAX_THREAD_NUM) threadNum = _MAX_THREAD_NUM;
    threads.resize(threadNum); // 先直接resize，避免后面再push_back时动态调整
    for (int i = 0; i < threadNum; ++i) threads[i] = new std::thread(std::bind(&ThreadPool::ThreadLoop, this));
    isRunning = true;
}

void ThreadPool::Stop()
{
    {
        std::unique_lock<std::mutex> lock(_myMutex);
        isRunning = false;
    }
    _myCon.notify_all();
    for (std::thread* worker: threads)
    {
        worker->join();
        delete worker;
    }
    threads.clear();
}

void ThreadPool::AddTask(Task* task)
{
    std::unique_lock<std::mutex> lock(_myMutex);
    tasks.push(task);
    _myCon.notify_one();
}

void ThreadPool::ThreadLoop()
{
    while (1)
    {
        Task* spTask;
        {
            std::unique_lock<std::mutex> lock(_myMutex);
            while (tasks.empty())
            {
                if (!isRunning) break;
                _myCon.wait(lock);
            }
            if (!isRunning) break;
            spTask = tasks.top();
            tasks.pop();
        }
        if (spTask == nullptr) continue;
        spTask->func();
        delete spTask;
    }
}

void _TestFun0()
{
    std::cout << "_TestFun0" << std::endl; 
}

void _doAddTask0(ThreadPool& ThreadPool)
{
    Task* task;
    for (int i = 0; i < 10; ++i)
    {
        task = new Task(i, _TestFun0);
        ThreadPool.AddTask(task);
    }
}

int main()
{
    ThreadPool threadPool;
    threadPool.Start();

    std::vector<std::thread> threads(5);
    threads[0] = std::thread(_doAddTask0, threadPool);
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    threadPool.Stop();
    // threads[1] = std::thread(_doAddTask1);
    // threads[2] = std::thread(_doAddTask2);
    // threads[3] = std::thread(_doAddTask3);
    // threads[4] = std::thread(_doAddTask4);
}