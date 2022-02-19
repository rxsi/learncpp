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



/*
memory_order_releaxed：
不保证变量间的执行顺序，只保证当前变量的store/load的原子性
因此适用于当个变量的原子性保证。
应用场景：程序计数器
*/
// #include <atomic>
// #include <thread>
// #include <assert.h>
// #include <vector>
// #include <iostream>

// // std::atomic<int> cnt = 0; 这种初始化方式报错, atomic(const atomic&) = delete;即禁止拷贝构造
// std::atomic<int> cnt(0); // 这种是直接初始化，调用的是类对应的有参构造函数
// // int cnt; // 这种是默认初始化，对于函数之外的内置变量初始化值为0,多线程同时修改时线程不安全

// void f()
// {
//     for (int n = 0; n < 1000; ++n)
//     {
//         cnt++; // 这也是保证了原子性，因为atomic有重载operator++ 和 operator--，但是默认的内存序是 memory_order_seq_cst（最严格），在此处只需要保证当前++操作的原子性，因此使用memory_order_relaxed即可
//         // cnt.fetch_add(1, std::memory_order_relaxed); // memory_order_relaxed只保证当前语句的原子性,不保证其他store\load之间的顺序
//     }
// }

// int main()
// {
//     std::vector<std::thread> v;
//     for (int n = 0; n < 10; ++n)
//     {
//         v.emplace_back(f);
//     }

//     for (auto& t : v)
//     {
//         t.join();
//     }
//     assert(cnt == 10000);
//     return 0;
// }

/*
memory_order_consume：
影响load行为（将值读取到寄存器），处于当前变量的load行为之后的且与当前变量相关的读取行为不会被优化到在该语句之前执行
p = ptr.load(std::memory_order_consume);
p += 1; // 次语句永远只会在load之后再执行
a = 123; // 有可能会优化到load之前
*/

/*
memory_order_acquire：
影响load行为，处于当前变量的load行为之后的所有与内存相关的操作都不会被优化到该变量之前执行
p = ptr.load(std::memory_order_acquire);
p += 1; // 不会优化到load之前
a = 123; // 不会优化到load之前
*/

/*
memory_order_release：
影响store行为，处于当前store行为之前的所有与内存相关的操作都不会优化到该store行为之后
std::string* p = new std::string("hello"); // 不会优化到store之后执行
data = 33; // 不会优化到store之后执行
ptr.store(p, std::memory_order_release);
*/

/*
memory_order_acq_rel：

*/