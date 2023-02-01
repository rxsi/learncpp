#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <random>

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10
#define MIN_THREAD_POOL 2
#define MAX_THREAD_POOL 5

class Task
{
public:
    Task(std::function<void(int)> fun, int priority = MIN_PRIORITY);
    int priority;
    std::function<void(int)> fun;
};

Task::Task(std::function<void(int)> fun, int priority): priority(priority), fun(fun) {} // priority越大则优先级越高

class ThreadNode
{
public:
    ThreadNode* prev;
    ThreadNode* next;
    std::thread t;
    ThreadNode(): prev(nullptr), next(nullptr) {}
};

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();
    void Start(int defaultPoolSize = MIN_THREAD_POOL); // 默认10个线程
    void Stop();

    void addTask(Task* task);

private:
    void ThreadLoopFun(ThreadNode* node);
    struct TaskCmp
    {
        bool operator()(const Task *t1, const Task *t2)
        {
            return t1->priority < t2->priority;
        }
    };

private:
    int defaultPoolSize;
    int curPoolSize;
    ThreadNode* head;
    ThreadNode* tail;
    bool isRunning; // 
    std::mutex lockMutex; // 一开始使用采用可重入锁，但是无法和condition_variable结合
    std::condition_variable lockCondition;
    std::priority_queue<Task*, std::vector<Task*>, TaskCmp> taskQue;
};

ThreadPool::ThreadPool(): isRunning(false)
{
    this->head = new ThreadNode();
    this->tail = new ThreadNode();
    this->head->next = this->tail;
    this->tail->prev = this->head;
}

ThreadPool::~ThreadPool() 
{
    Stop();
}

void ThreadPool::Stop()
{
    {
        std::lock_guard<std::mutex> lock(this->lockMutex);
        if (!this->isRunning) return; // 避免重复调用
        this->isRunning = false; // 别的线程有用到这个变量，因此在修改前需要获取到锁
    }
    this->lockCondition.notify_all();
    ThreadNode* cur = head;
    ThreadNode* curNext;
    while (cur != nullptr)
    {
        if (cur->t.joinable()) cur->t.join(); // 任务没有运行完那么就等待运行完
        curNext = cur->next;
        delete cur;
        cur = curNext;
    }
    this->curPoolSize = 0;
    while (!taskQue.empty())
    {
        Task* t = taskQue.top();
        taskQue.pop();
        delete t;
    }
}

void ThreadPool::Start(int defaultPoolSize)
{
    if (this->isRunning) return; // 避免重复启动
    this->defaultPoolSize = std::max(std::min(defaultPoolSize, MAX_THREAD_POOL), MIN_THREAD_POOL);
    ThreadNode* cur = head;
    ThreadNode* curNext;
    while (this->curPoolSize < this->defaultPoolSize)
    {
        ThreadNode* node = new ThreadNode();
        node->t = std::thread(&ThreadPool::ThreadLoopFun, this, node);
        curNext = cur->next;
        cur->next = node;
        node->prev = cur;
        node->next = curNext;
        curNext->prev = node;
        cur = node;
        this->curPoolSize++;
    }
    this->isRunning = true;
}

void ThreadPool::addTask(Task* task)
{
    {
        std::lock_guard<std::mutex> lock(this->lockMutex);
        if (this->taskQue.size() > MAX_THREAD_POOL && this->curPoolSize < MAX_THREAD_POOL) // 可进行容量拓展
        {
            std::cout << "add new thread" << std::endl;
            ThreadNode* cur = tail->prev;
            ThreadNode* curNext;
            while (this->curPoolSize < MAX_THREAD_POOL)
            {
                ThreadNode* node = new ThreadNode();
                node->t = std::thread(&ThreadPool::ThreadLoopFun, this, node);
                curNext = cur->next;
                cur->next = node;
                node->prev = cur;
                node->next = curNext;
                curNext->prev = node;
                cur = node;
                this->curPoolSize++;
            }
        }
        taskQue.push(task);
    }
    this->lockCondition.notify_one();
}

void ThreadPool::ThreadLoopFun(ThreadNode* cur)
{
    while (true)
    {
        if (!this->isRunning) break;
        Task* task;
        {
            std::unique_lock<std::mutex> lock(this->lockMutex); // 配合条件变量使用必须使用unique_lock，它可以交出锁的控制权
            this->lockCondition.wait_for(lock, std::chrono::seconds(30)); // 最多等待30秒，这里有可能出现被系统中断
            if (taskQue.empty()) break; // 虽然可能是被系统中断了，但是这里直接当失败了
            task = this->taskQue.top();
            taskQue.pop();
        }
        task->fun(task->priority);
        delete task;
    }
    // 超出了循环就要判断是否需要动态缩减线程数
    std::unique_lock<std::mutex> lock(this->lockMutex);
    if (this->taskQue.size() < MIN_THREAD_POOL && this->curPoolSize > MIN_THREAD_POOL)
    {
        std::cout << "can del" << std::endl;
        ThreadNode* curNext = cur->next;
        ThreadNode* curPrev = cur->prev;
        curPrev->next = curNext;
        curNext->prev = curPrev;
        this->curPoolSize--;
        delete cur;
    }
}

void test_fun(int priority)
{
    std::cout << "test_fun in threads: " << std::this_thread::get_id() << ", priority = " << priority << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(rand()%10 + 1)); // 随机休眠
}

void task_fun(ThreadPool& threadPool)
{
    for (int i = 0; i < 20; ++i)
    {
        int priority = rand() % 10 + 1;
        Task* t1 = new Task(test_fun, priority);
        threadPool.addTask(t1);
    }
}

int main()
{
    ThreadPool threadPool;
    threadPool.Start();
    std::thread t(task_fun, std::ref(threadPool));
    t.join();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    threadPool.Stop();
}
