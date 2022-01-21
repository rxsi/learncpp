// linux 平台的底层线程函数
// g++ thread_test.cpp -o thread_test -lpthread
// #include <pthread.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <iostream>

// void* threadfunc(void* args)
// {
//     while(1)
//     {
//         sleep(1);
//         std::cout << "Linux thread func" << std::endl;
//         break;
//     }
//     return NULL;
// }

// int main()
// {
//     pthread_t threadid;
//     pthread_create(&threadid, NULL, threadfunc, NULL);

//     pthread_join(threadid, NULL);
//     return 0;
// }


// #include <stdio.h>
// #include <thread>
// #include <iostream>

// void threadproc1()
// {
//     std::cout << "C++ std::thread 1" << std::endl;
// }

// void threadproc2(int a, int b)
// {
//     std::cout << "C++ std::thread 2" << std::endl;
// }

// int main()
// {
//     std::thread t1(threadproc1);
//     std::thread t2(threadproc2, 1, 2);
//     t1.join();
//     t2.join();
//     return 0;
// }


#include <thread>
#include <memory>
#include <stdio.h>
#include <iostream>

class Thread
{
public:
    Thread(){}
    ~Thread(){}
    void Start()
    {
        m_stopped = false;
        m_spThread.reset(new std::thread(&Thread::threadFunc, this, 1, 2)); // 注意,因为调用的是实例对象,因此要主动添加this指针作为参数
    }

    void Stop()
    {
        m_stopped = true;
        if (m_spThread)
        {
            if (m_spThread->joinable())
            {
                m_spThread->join();
            }
        }
    }

private:

    void threadFunc(int arg1, int arg2)
    {
        while (!m_stopped)
        {
            std::cout << "Thread function use instance method." << std::endl;
        }
    }

    std::shared_ptr<std::thread> m_spThread;
    bool m_stopped;
};


int main()
{
    Thread myThread;
    myThread.Start();
}