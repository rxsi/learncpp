// linux 平台的底层线程函数
// 可使用 pstack <pid> 查看进程每个线程的当前调用栈
// 注意要加 -lpthread编译参数,告知编译器使用pthread.h库进行链接
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

// #include <sys/syscall.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <pthread.h>

// void* thread_proc(void* args)
// {
//     pthread_t* tid1 = (pthread_t*) args; // 输出指向线程的内存地址，不是全系统唯一的，因为可能不同进程共享了同一块内存
//     int tid2 = syscall(SYS_gettid); // 全系统唯一，是LWP的ID（轻量级进程，早期linux系统的线程是通过进程实现的）
//     pthread_t tid3 = pthread_self(); // 输出指向线程的内存地址，不是全系统唯一的，因为可能不同进程共享了同一块内存

// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, NULL, thread_proc, &tid);
//     pthread_join(tid, NULL);
//     return 0;
// }

#include <atomic>
#include <thread>
#include <assert.h>
#include <vector>
#include <iostream>

// std::atomic<int> cnt = 0; 这种初始化方式报错, atomic(const atomic&) = delete;即禁止拷贝构造
std::atomic<int> cnt(0);

void f()
{
    for (int n = 0; n < 1000; ++n)
    {
        cnt.fetch_add(1, std::memory_order_relaxed); // memory_order_relaxed只保证当前语句的原子性,不保证其他store\load之间的顺序
    }
}

int main()
{
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n)
    {
        v.emplace_back(f);
    }

    for (auto& t : v)
    {
        t.join();
    }

    assert(cnt == 10000);
    return 0;
}