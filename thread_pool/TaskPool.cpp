
#include "TaskPool.h"
#include <functional>

TaskPool::TaskPool(): m_bRunning(false){}

TaskPool::~TaskPool()
{
    removeAllTasks(); // 因为线程池是创建在栈空间,因此当线程池所在的线程退出时会调用线程池对象的析构函数,当然如果定义在堆内存,则需要手动调用delete,或者是使用shared_ptr
}

// 默认值经过编译之后,实际上是在调用函数时传入了这个默认值,实际上的汇编代码和普通函数一样
void TaskPool::init(int threadNum) // 默认值问题,这里不能加默认值,也就是说只能在声明时加默认值,定义时不能加默认值
{
    if (threadNum <= 0);
        threadNum = 5;

    m_bRunning = true;
    for (int i = 0; i < threadNum; ++i)
    {
        auto spThread = std::make_shared<std::thread>(std::bind(&TaskPool::threadFunc, this)); // 启动之后会创建5条线程,五条线程都会等待task的生产
        // std::shared_ptr<std::thread> spThread;
        // spThread.reset(new std::thread(std::bind(&TaskPool::threadFunc, this)));
        m_threads.push_back(spThread);
    }
}

void TaskPool::threadFunc()
{
    std::shared_ptr<Task> spTask;
    while (true)
    {
        { // 减小guard锁的作用范围
            std::unique_lock<std::mutex> guard(m_mutexList);
            while (m_TaskList.empty())
            {
                if (!m_bRunning)
                    break;
                m_cv.wait(guard); // 获得互斥锁但是条件没有达成,则会释放锁且挂起本线程,其他线程可以继续执行,当满足条件之后会再被唤醒
            }

            if (!m_bRunning)
                break;
            spTask = m_TaskList.front();
            m_TaskList.pop_front();
        }
        if (spTask == NULL)
            continue;
        // spTask.doIt(); 注意这种访问方式是在 std::shared_ptr<Task>中寻找doIt方法,会报错
        // (*spTask).doIt();
        spTask->doIt(); // 这种方式和上面那种方式访问的将是Task的作用域,因为shared_ptr重载了 operator* 和 operator-> 方法
        spTask.reset(); // 这里其实可以不用reset,因为已经从m_TaskList中pop出来,也就是说这里是最后唯一的引用了,因此当函数退出后,会自动释放该对象
    }

    std::cout << "exit thread, threadID: " << std::this_thread::get_id() << std::endl;
}

void TaskPool::stop()
{
    m_bRunning = false;
    m_cv.notify_all();

    for (auto& iter : m_threads) // 确保线程池中的线程都运行完成再退出线程池所在的线程
    {
        if (iter->joinable())
            iter->join();
    }
}

void TaskPool::addTask(Task* task)
{
    // auto spTask = std::make_shared<Task>(task); 这里只能使用reset,因为task本身就是指针,只能进行重定向
    std::shared_ptr<Task> spTask;
    spTask.reset(task);

    {
        std::lock_guard<std::mutex> guard(m_mutexList);
        m_TaskList.push_back(spTask);
        std::cout << "add a Task." << std::endl;
    }

    m_cv.notify_one();
}

void TaskPool::removeAllTasks()
{
    {
        std::lock_guard<std::mutex> guard(m_mutexList);
        for (auto& iter : m_TaskList) // 这里其实可以不用,因为是shared_ptr,当调用list.clear时就会使对象释放
            iter.reset();

        m_TaskList.clear();
    }
}


// g++ -g main.cpp TaskPool.h TaskPool.cpp -o main -lpthread