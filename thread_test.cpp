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

/* release + comsume 构成了 Carries dependency关系 */
// #include <thread>
// #include <atomic>
// #include <assert.h>
// #include <string>

// std::atomic<std::string*> ptr;
// int data;

// void producer()
// {
//     std::string* p = new std::string("Hello");
//     data = 22;
//     ptr.store(p, std::memory_order_release); // store操作属于把数据写回内存，因此需要把前面的内存操作都先执行完
// }

// void consumer()
// {
//     std::string* p2;
//     while (!(p2 = ptr.load(std::memory_order_consume))); // load操作属于把数据从内存读到寄存器，
//     assert(*p2 == "Hello"); // 永远不会失败
//     assert(data == 42);// 可能失败,因为可能被提前到while之前做了判断。因为是release，因此while之后是保证data==42的
// }

// int main()
// {
//     std::thread t1(producer);
//     std::thread t2(consumer);
//     t1.join();
//     t2.join();
// }

/* release + acquire 构成了 synchronize-with 关系 */
// #include <thread>
// #include <atomic>
// #include <assert.h>
// #include <string>
// #include <iostream>

// std::atomic<bool> ready{false};
// int data{0};
// std::atomic<int> var{0};

// void sender()
// {
//     data = 22;
//     var.store(100, std::memory_order_relaxed);
//     ready.store(true, std::memory_order_release); // 本语句之前的内存操作语句都不会被优化到本store之后
// }

// void receiver()
// {
//     while(!ready.load(std::memory_order_acquire)); // 在这之后的所有的内存操作都不能在被优化到本load语句之前
//     assert(data == 22); // 永远都成功
//     assert(var == 100); // 永远都成功
// }

// int main()
// {
//     std::thread t1(sender);
//     std::thread t2(receiver);
//     t1.join();
//     t2.join();
// }

/*
memory_order_acq_rel：
同时影响store-load的行为，即在他之前的任何内存操作不能优化到他之后，而原本在他之后的语句不能优化到他之前
一般用在 RMW 操作语境
*/
// #include <thread>
// #include <atomic>
// #include <assert.h>
// #include <vector>

// std::vector<int> data;
// std::atomic<int> flag{0};
// int num;

// void thread_1()
// {
//     data.push_back(42);
//     flag.store(1, std::memory_order_release);
// }

// void thread_2()
// {
//     // 对于RMW（Read-Modify-Writes，需要实现原子性）操作，属于把三个操作绑定为一个原子性操作，因此需要使用memory_order_acq_rel 或者 memory_order_seq_cst 
//     // compare_exchange_weak(T& expected, T desired, std::memory_order order = std::memory_order_seq_cst) 
//     // compare_exchange_strong(T& expected, T desired, std::memory_order order = std::memory_order_seq_cst) 
//     // 作用都是比较*this和 expected值，如果二者相等，那么把*this替换为 desired，并返回true（RMW 操作）
//     // 如果不想等则把expected赋值为*this的值（load 操作）
//     // 不同的是weak可能会因为（fail spuriously)而导致返回错误结果，因此需要放在loop中判断（如while）
//     int expected = 1; // 由于memory_order_acq_rel，因此expected会比下面的语句优先执行，即=1
//     num = 22; // 这个语句也不会被重排到后面执行
//     while(!flag.compare_exchange_strong(expected, 2, std::memory_order_acq_rel)) // 是当flag的值与expected不一致时，代表thread_1还没有执行，flag = 0,会使expected被置为0，因此需要在循环体内继续把expected设置为1
//     {
//         expected = 1;
//     }
//     assert(num == 22);
// }

// void thread_3()
// {
//     while(flag.load(std::memory_order_acquire) < 2);
//     assert(data.at(0) == 42);
//     assert(num == 22);
// }

// int main()
// {
//     for (int i = 0; i < 100; ++i)
//     {
//         std::thread a(thread_1);
//         std::thread b(thread_2);
//         std::thread c(thread_3);
        
//         a.join();
//         b.join();
//         c.join();
//     }  
// }


/*
memory_order_seq_cst：
当使用该内存序时，同为 memory_order_seq_cst 内存序的变量会以一个简单序列运行，且变量前后所有关于内存的操作都不会被优化重排到该变量之前或之后
最主要的是：变量A如果是在变量B之前执行，那么变量A的前后内存操作语句都会在B之前执行。（memory_order_acq_rel只保证相同变量间的关系）
且 RMW 操作会保证永远读取的是最新的值！！！！！！！！
*/

#include <thread>
#include <atomic>
#include <assert.h>
#include <vector>

std::atomic<bool> x{false};
std::atomic<bool> y{false};
std::atomic<int> z{0};

void write_x()
{
    x.store(true, std::memory_order_seq_cst);
}

void write_y()
{
    y.store(true, std::memory_order_seq_cst);
}

void read_x_then_y()
{
    while (!x.load(std::memory_order_seq_cst)); // 如果不使用 memory_order_seq_cst，即无法保证 y.load 在  while (!y.load(std::memory_order_seq_cst));之前执行，
    // 那么就可能出现，线程c、d同时把x，y都读取到寄存器，然后同时执行完while语句之后，根据自身寄存器状态，发现x/y是0,因此最终z = 0
    if (y.load(std::memory_order_seq_cst))
    {
        ++z;
    }
}

void read_y_then_x()
{
    while (!y.load(std::memory_order_seq_cst));
    if (x.load(std::memory_order_seq_cst))
    {
        ++z;
    }
}

int main()
{
    std::thread a(write_x);
    std::thread b(write_y);
    std::thread c(read_x_then_y);
    std::thread d(read_y_then_x);
    a.join();
    b.join();
    c.join();
    d.join();
    assert(z.load() != 0);
}