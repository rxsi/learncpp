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
// #include <vector>
// #include <ctime>

// void threadproc1()
// {
//     std::this_thread::sleep_for(std::chrono::seconds(1));
//     // std::cout << "C++ std::thread 1" << std::endl;
// }

// void start_fun()
// {
//     for (int i = 0; i < 5; ++i)
//     {
//         std::thread t(threadproc1);
//         t.join();
//     }
// }

// void start_fun2()
// {
//     std::vector<std::thread> threads;
//     for (int i = 0; i < 5; ++i)
//     {
//         threads.emplace_back(std::thread(threadproc1));
//     }
//     for (int i = 0; i < 5; ++i)
//     {
//         threads[i].join();
//     }
// }

// int main()
// {
//     time_t now = time(0);
//     std::cout << "start_time: " << now << std::endl;
//     start_fun();
//     now = time(0);
//     std::cout << "end_time: " << now << std::endl;
//     std::cout << "start_time: " << now << std::endl;
//     start_fun2();
//     now = time(0);
//     std::cout << "end_time: " << now << std::endl;
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

#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <iostream>
using namespace std;

void* thread_proc(void* args)
{
    pthread_t* tid1 = (pthread_t*) args; // 输出指向线程的内存地址，不是全系统唯一的，因为可能不同进程共享了同一块内存
    cout << "tid = " << *tid1 << endl;
    int tid2 = syscall(SYS_gettid); // 全系统唯一，是LWP的ID（轻量级进程，早期linux系统的线程是通过进程实现的）
    cout << "tid2 = " << tid2 << endl;
    pthread_t tid3 = pthread_self(); // 输出指向线程的内存地址，不是全系统唯一的，因为可能不同进程共享了同一块内存
    cout << "tid3 = " << (int)tid3 << endl;
    pid_t pid = getpid();
    cout << "pid = " << (int)pid << endl;
    return NULL;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread_proc, &tid);
    pthread_join(tid, NULL);
    return 0;
}



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

// #include <thread>
// #include <atomic>
// #include <assert.h>
// #include <vector>

// std::atomic<bool> x{false};
// std::atomic<bool> y{false};
// std::atomic<int> z{0};

// void write_x()
// {
//     x.store(true, std::memory_order_seq_cst);
// }

// void write_y()
// {
//     y.store(true, std::memory_order_seq_cst);
// }

// void read_x_then_y()
// {
//     while (!x.load(std::memory_order_seq_cst)); // 如果不使用 memory_order_seq_cst，即无法保证 y.load 在  while (!y.load(std::memory_order_seq_cst));之前执行，
//     // 那么就可能出现，线程c、d同时把x，y都读取到寄存器，然后同时执行完while语句之后，根据自身寄存器状态，发现x/y是0,因此最终z = 0
//     if (y.load(std::memory_order_seq_cst))
//     {
//         ++z;
//     }
// }

// void read_y_then_x()
// {
//     while (!y.load(std::memory_order_seq_cst));
//     if (x.load(std::memory_order_seq_cst))
//     {
//         ++z;
//     }
// }

// int main()
// {
//     std::thread a(write_x);
//     std::thread b(write_y);
//     std::thread c(read_x_then_y);
//     std::thread d(read_y_then_x);
//     a.join();
//     b.join();
//     c.join();
//     d.join();
//     assert(z.load() != 0);
// }

/*
linux 互斥体

初始化
#include <pthread.h>
pthread_mutex_t = PTHREAD_MUTEX_INITIALIZER; // 默认属性
int pthread_mutex_init(pthread_mutex_t* restrict mutex, const pthread_mutexattr_t* restrict attr); // 可以指定属性，restrict用以修饰后面的指针可以直接从寄存器读取，只能用以修饰指针
一般属性直接使用 NULL 即可，即默认属性

设置属性
int pthread_mutexattr_init(pthread_mutexattr_t* attr); // 创建属性对象
int pthread_mutexattr_destory(pthread_mutexattr_t* attr); 当设置属性之后需要进行销毁
int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int type); // 设置属性
int pthread_mutexattr_gettype(const pthread_mutexattr_t* restrict attr, int* restrict type); 获取属性

可选属性：
1.PTHREAD_MUTEX_NORMAL：
普通锁，不同线程只有该锁被释放之后才能再被获取
如果同一个线程连续加锁会导致阻塞在第二个加锁语句，而如果使用的是trylock，则返回EBUSY错误码

2.PTHREAD_MUTEX_ERRORCHECK：
检错锁，不同线程只有该锁被释放之后才能再被获取
如果同一个线程连续加锁会返回EDEADLK错误码

3.PTHREAD_MUTEX_RECURSIVE：
可重入锁，不同线程必须等锁被释放之后才能再次加锁
同一个线程重复加锁会使锁的引用计数+1,每次调用解锁则是引用计数-1,因此需要成对出现

销毁
int pthread_mutex_destory(pthread_mutex_t* mutex); // 执行成功会返回0
注意两点：1）无须销毁PTHREAD_MUTEX_INITIALIZER初始化的互斥体;2）不要销毁一个已经加锁或者正在被条件变量使用的互斥体对象，否则会返回EBUSY错误

加锁解锁
int pthread_mutex_lock(pthread_mutex_t* mutex);
int pthread_mutex_trylock(pthread_mutex_t* mutex);
int pthread_mutex_unlock(pthread_mutex_t* mutex);
执行成功会返回0，
*/

// #include <pthread.h>
// #include <stdio.h>
// #include <errno.h>
// #include <iostream>
// #include <unistd.h>

// pthread_mutex_t mymutex;
// pthread_mutex_t mymutex2;

// void* thread_fun(void* args)
// {
//     pthread_t* threadID = (pthread_t*) args;
//     std::cout << threadID << ": lock in mutext" << std::endl;
//     pthread_mutex_lock(&mymutex);
//     std::cout << threadID << ": thread running" << std::endl;
//     sleep(3);
//     std::cout << threadID << ": try to lock mymutext2" << std::endl;
//     pthread_mutex_lock(&mymutex2);
    
//     // 尝试销毁被锁定的mutex对象
//     int  ret = pthread_mutex_destroy(&mymutex);
//     if (ret != 0)
//     {
//         if (ret == EBUSY)
//         {
//             std::cout << "EBUSY" << std::endl;
//             std::cout << "Failed to destory mutex." << std::endl;
//         }
//     }
//     pthread_mutex_unlock(&mymutex);
//     return NULL;
// }

// void* thread_fun2(void* args)
// {
//     pthread_t* threadID = (pthread_t*) args;
//     std::cout << threadID << ": lock in mutext2" << std::endl;
//     pthread_mutex_lock(&mymutex2);
//     std::cout << threadID << ": thread running" << std::endl;
//     sleep(3);
//     std::cout << threadID << ": try to lock mymutext" << std::endl;
//     pthread_mutex_lock(&mymutex);
    
//     // 尝试销毁被锁定的mutex对象
//     int  ret = pthread_mutex_destroy(&mymutex);
//     if (ret != 0)
//     {
//         if (ret == EBUSY)
//         {
//             std::cout << "EBUSY" << std::endl;
//             std::cout << "Failed to destory mutex." << std::endl;
//         }
//     }
//     pthread_mutex_unlock(&mymutex);
//     return NULL;
// }

// int main()
// {
//     pthread_mutex_init(&mymutex, NULL); // 一般不用检查初始化结果
//     pthread_mutex_init(&mymutex2, NULL); // 一般不用检查初始化结果
//     pthread_t threadID1, threadID2, threadID3;
//     pthread_create(&threadID1, NULL, thread_fun, &threadID1);
//     pthread_create(&threadID2, NULL, thread_fun2, &threadID2);
//     // pthread_create(&threadID3, NULL, thread_fun, &threadID3);
//     pthread_join(threadID1, NULL);
//     pthread_join(threadID2, NULL);
//     // pthread_join(threadID3, NULL);
//     // 尝试销毁已经解锁的mutex对象
//     int ret = pthread_mutex_destroy(&mymutex);
//     int ret2 = pthread_mutex_destroy(&mymutex2);
//     if (ret == 0 && ret2 == 0)
//     {
//         std::cout << "Successed to destory mutex." << std::endl;
//     }
//     return 0;
// }

/*
linux 信号量
#include <semaphore.h>
int sem_init(sem_t* sem, int pshared, unsigned int value); 
pshared代表的是是否可以被不同进程共享，0否，非0是
value代表初始时的资源量

int sem_destory(sem_t* sem);
销毁信号量

int sem_post(sem_t* sem);
使信号量的资源数+1,且唤醒所有因sem_wait函数被阻塞的其他线程都会被唤醒

int sem_wait(sem_t* sem);
当信号量为0时会阻塞，当信号量大于0时会被唤醒，唤醒后会把资源计数-1,然后立即返回执行后面的操作

int sem_trywait(sem_t* sem);
当信号量为0时会立即返回，返回值是-1,错误码errno == EAGAIN

int sem_timedwait(sem_t* sem, const struct timespec* abs_timeout);
带有等待时间的等待，超时后返回-1,错误码 errno == ETIMEDOUT

三个wait函数都会被系统中断，会立即返回，返回值-1,错误码 errno == EINTR

struct timespec
{
    time_t tv_sec; 秒
    long tv_nsec; 纳秒
}
注意temespec是绝对时间

*/

// #include <pthread.h>
// #include <errno.h>
// #include <unistd.h>
// #include <list>
// #include <semaphore.h>
// #include <iostream>

// class Task
// {
// public:
//     Task(int myTaskID): taskID(myTaskID){}

//     void doTask()
//     {
//         std::cout << "handle a task, taskID: " << taskID << ", threadID: " << pthread_self() << std::endl;
//     }

// private:
//     int taskID;
// };

// pthread_mutex_t mymutex;
// std::list<Task*> tasks;
// sem_t mysemapthore;

// void* consumer_thread(void* param)
// {
//     Task* pTask = NULL;
//     while (true)
//     {
//         if (sem_wait(&mysemapthore) != 0)
//             continue;
//         if (tasks.empty())
//             continue;

//         pthread_mutex_lock(&mymutex);
//         pTask = tasks.front();
//         tasks.pop_front();
//         pthread_mutex_unlock(&mymutex);

//         pTask->doTask();
//         delete pTask;
//     }

//     return NULL;
// }

// void* producer_thread(void* param)
// {
//     int taskID = 0;
//     Task* pTask = NULL;

//     while (true)
//     {
//         pTask = new Task(taskID);
//         pthread_mutex_lock(&mymutex);
//         tasks.push_back(pTask);
//         std::cout << "produce a task, taskID: " << taskID << ", threadID: " << pthread_self() << std::endl;
//         pthread_mutex_unlock(&mymutex);

//         // 释放信号量，通知消费者
//         sem_post(&mysemapthore);
//         taskID++;
//         sleep(1);
//     }
//     return NULL;
// }

// int main()
// {
//     pthread_mutex_init(&mymutex, NULL); // 用以保证tasks列表操作的安全性
//     sem_init(&mysemapthore, 0, 0);

//     // 五个消费线程
//     pthread_t consumerThreadID[5];
//     for (int i = 0; i < 5; i++)
//         pthread_create(&consumerThreadID[i], NULL, consumer_thread, NULL);

//     // 一个生产者线程
//     pthread_t producerThreadID;
//     pthread_create(&producerThreadID, NULL, producer_thread, NULL);

//     pthread_join(producerThreadID, NULL);

//     for (int i = 0; i < 5; ++i)
//         pthread_join(consumerThreadID[i], NULL);

//     sem_destroy(&mysemapthore);
//     pthread_mutex_destroy(&mymutex);

//     return 0;
// }

/*
linux 条件变量


初始化
int pthread_cond_init(pthread_cond_t* cond, const pthread_condattr_t* attr);
或者：
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

销毁
int pthread_cond_destory(pthread_cond_t* cond);

等待
int pthread_cond_wait(pthread_cond_t* restrict cond, pthread_mutex_t* restrict mutex);
int pthread_cond_timewait(pthread_cond_t* restrict cond, pthred_mutex_t* restrict mutex, const struct timespec* restrict abstime);
因为信号变量必须在信号发出时，线程已经有调用wait操作，因此需要把wait和互斥体的unlock变成一个原子操作
否则可能出现互斥体刚unlock，然后失去了时间片，而另外的线程发出信号，切换回本线程之后已经失去了信号了，造成永久阻塞
当条件未满足时，线程会一直等待
注意temespec是绝对时间



唤醒
int pthread_cond_signal(pthread_cond_t* cond);  // 一次唤醒一个线程
int pthread_cond_broadcast(pthread_cond_t* cond); // 同时唤醒所有的线程

*/

// #include <pthread.h>
// #include <errno.h>
// #include <unistd.h>
// #include <list>
// #include <semaphore.h>
// #include <iostream>

// class Task
// {
// public:
//     Task(int myTaskID): taskID(myTaskID){}

//     void doTask()
//     {
//         std::cout << "handle a task, taskID: " << taskID << ", threadID: " << pthread_self() << std::endl;
//     }

// private:
//     int taskID;
// };

// pthread_mutex_t mymutex;
// std::list<Task*> tasks;
// pthread_cond_t mycv;

// void* consumer_thread(void* param)
// {
//     Task* pTask = NULL;
//     while (true)
//     {
//         pthread_mutex_lock(&mymutex);

//         // 这里使用while的原因是可能会被虚假唤醒，因此当wait被唤醒之后，还需要判断tasks是否为空
//         while (tasks.empty())
//             // 如果获得了互斥锁，但是条件不合适，则pthread_cond_wait 会释放锁，不往下执行，所以在这语句之前有lock的操作
//             // 发生变化后，如果条件合适，则pthread_cond_wait会阻塞到获得锁，所以在这语句之后有unlock的操作
//             pthread_cond_wait(&mycv, &mymutex);


//         pTask = tasks.front();
//         tasks.pop_front();

//         pthread_mutex_unlock(&mymutex);

//         if (pTask == NULL)
//             continue;

//         pTask->doTask();
//         delete pTask;
//         pTask = NULL;
//     }

//     return NULL;
// }

// void* producer_thread(void* param)
// {
//     int taskID = 0;
//     Task* pTask = NULL;

//     while (true)
//     {
//         pTask = new Task(taskID);
//         pthread_mutex_lock(&mymutex);
//         tasks.push_back(pTask);
//         std::cout << "produce a task, taskID: " << taskID << ", threadID: " << pthread_self() << std::endl;
//         pthread_mutex_unlock(&mymutex);

//         pthread_cond_signal(&mycv);
//         taskID++;
//         sleep(1);
//     }
//     return NULL;
// }

// int main()
// {
//     pthread_mutex_init(&mymutex, NULL); // 用以保证tasks列表操作的安全性
//     pthread_cond_init(&mycv, NULL);

//     // 五个消费线程
//     pthread_t consumerThreadID[5];
//     for (int i = 0; i < 5; i++)
//         pthread_create(&consumerThreadID[i], NULL, consumer_thread, NULL);

//     // 一个生产者线程
//     pthread_t producerThreadID;
//     pthread_create(&producerThreadID, NULL, producer_thread, NULL);

//     pthread_join(producerThreadID, NULL);

//     for (int i = 0; i < 5; ++i)
//         pthread_join(consumerThreadID[i], NULL);

//     pthread_cond_destroy(&mycv);
//     pthread_mutex_destroy(&mymutex);

//     return 0;
// }

/*
linux 读写锁
读写锁和互斥体的差别在于
读锁和读锁是共享的
而写锁和写锁，写锁和读锁都是互斥的

#include <pthread.h>

初始化
int pthread_rwlock_init(pthread_rwlock_t* rwlock, const pthread_rwlockattr_t* attr); 属性一般为NULL即可

属性设置
int pthread_rwlockattr_setkind_np(pthread_rwlockattr_t* attr, int pref);
int pthread_rwlockattr_getkind_np(const pthread_rwlockattr_t* attr, int* pref);

pref参数用于设置读写锁的类型，取值如下：
enum
{
    // 读者优先
    PTHREAD_RWLOCK_PREFER_READER_NP,
    PTHREAD_RWLOCK_PREFER_WRITER_NP,

    // 写者优先
    PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,

    PTHREAD_RWLOCK_DEFAULT_NP = PTHREAD_RWLOCK_PREFER_READER_NP （默认属性，依然是读者优先）
}

属性初始化和销毁
int pthread_rwlockattr_init(pthread_rwlockattr_t* attr);
int pthread_rwlockattr_destory(pthread_rwlockattr_t* attr);


或者使用下方的初始化方式
pthread_rwlock_t myrwlock = PTHREAD_RWLOCK_INITIALIZED;


销毁
int pthread_rwlock_destory(pthread_rwlock_t* rwlock);


请求读锁
int pthread_rwlock_rdlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t* rwlock); // 当不能获取锁时返回-1,errno == EBUSY
int pthread_rwlock_timedrdlock(pthread_rwlock_t* rwlock, const struct timespec* abstime); // 超时后返回 -1 && errno == ETIMEOUT

请求写锁
int pthread_rwlock_wrlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t* rwlock); // 当不能获取锁时返回-1,errno == EBUSY
int pthread_rwlock_timedwrlock(pthread_rwlock_t* rwlock, const struct timespec* abstime); //超时后返回 -1 && errno == ETIMEOUT

释放锁
int pthread_rwlock_unlock(pthread_rwlock_t* rwlock);

*/

// #include <pthread.h>
// #include <unistd.h>
// #include <iostream>

// int resourceID = 0;
// pthread_rwlock_t myrwlock;

// void* read_thread(void* param)
// {
//     while (true)
//     {
//         pthread_rwlock_rdlock(&myrwlock);
//         std::cout << "read thread ID: " << pthread_self() << ", resourceID: " << resourceID << std::endl;

//         sleep(1);
//         pthread_rwlock_unlock(&myrwlock);
//     }
//     return NULL;
// }

// void* write_thread(void* parm)
// {
//     while (true)
//     {
//         pthread_rwlock_wrlock(&myrwlock);

//         ++resourceID;
//         std::cout << "write thread ID: " << pthread_self() << ", resourceID: " << resourceID << std::endl;

//         sleep(1);
//         pthread_rwlock_unlock(&myrwlock);
//     }

//     return NULL;
// }

// int main()
// {
//     pthread_rwlock_init(&myrwlock, NULL); // 由于默认是读优先，因此写线程很难获得机会执行
//     pthread_t readThreadID[5];
//     for (int i = 0; i < 5; ++i)
//         pthread_create(&readThreadID[i], NULL, read_thread, NULL);

//     pthread_t writeThreadID;
//     pthread_create(&writeThreadID, NULL, write_thread, NULL);
//     pthread_join(writeThreadID, NULL);
//     for (int i = 0; i < 5; ++i)
//         pthread_join(readThreadID[i], NULL);

//     pthread_rwlock_destroy(&myrwlock);
//     return 0;
// }

/*
std::mutex 互斥体
windows平台和linux平台通用
正常的做法是 lock 和 unlock 成对出现,为了避免出现差错,因此使用RAII封装了几个接口,类似于智能指针的做法

lock_guard      基于作用域的互斥体管理,即使用{}划定范围,当调用时会自动调用 std::mutex 的lock方法,当出了作用域时会自动调用 std::mutex 的unlock方法
unique_lock     灵活的互斥体管理,可以转让控制权等
shared_lock     共享互斥体管理
scoped_lock     多互斥体避免死锁管理尾


void fun()
{
    std::mutex m; // 这种方法是错的,因为std::mutex的作用域一定要超过fun()的作用域
    std::lock_guard<std::mutex> guard(m);
}

*/

// #include <iostream>
// #include <chrono>
// #include <thread>
// #include <mutex>
// #include <atomic>

// int g_num = 0;
// std::mutex g_num_mutex;

// void slow_increment(int id)
// {
//     for (int i = 0; i < 3; ++i)
//     {
//         g_num_mutex.lock(); // 如果连续重复加锁会导致阻塞
//         ++g_num;
//         std::cout << id << " => " << g_num << std::endl;
//         g_num_mutex.unlock();

//         std::this_thread::sleep_for(std::chrono::seconds(1)); // 休眠1秒
//     }
// }

// int main()
// {
//     std::thread t1(slow_increment, 0);
//     std::thread t2(slow_increment, 1);
//     t1.join();
//     t2.join();
//     return 0;
// }


/*
std::shared_mutex
底层是操作系统提供的读写锁

lock / unlock 用来获取写锁和解除写锁(排他锁)
lock_shared / unlock_shared 用来获取读锁和解除读锁(共享锁)

unique_lock(写锁) 和 shared_lock(读锁) 用来以RAII方式自动对std::shared_mutex加锁和解锁

*/

// #define READER_THREAD_COUNT 8
// #define LOOP_COUNT 5000000

// #include <iostream>
// #include <mutex> // 互斥体
// #include <shared_mutex> // 共享互斥体
// #include <thread>

// class shared_mutex_counter
// {
// public:
//     shared_mutex_counter() = default;
//     ~shared_mutex_counter() = default;

//     unsigned int get() const
//     {
//         std::shared_lock<std::shared_mutex> lock(m_mutex); // 共享锁
//         return m_value;
//     }

//     void increment()
//     {
//         std::unique_lock<std::shared_mutex> lock(m_mutex); // 排他锁
//         m_value++;
//     }

//     void reset()
//     {
//         std::unique_lock<std::shared_mutex> lock(m_mutex);
//         m_value = 0;
//     }

// private:
//     mutable std::shared_mutex m_mutex;
//     unsigned int m_value = 0;
// };


// class mutex_counter
// {
// public:
//     mutex_counter() = default;
//     ~mutex_counter() = default;

//     unsigned int get() const
//     {
//         std::unique_lock<std::mutex> lock(m_mutex); // 互斥体,相当于排他锁
//         return m_value;
//     }

//     void increment()
//     {
//         std::unique_lock<std::mutex> lock(m_mutex);
//         m_value++;
//     }

//     void reset()
//     {
//         std::unique_lock<std::mutex> lock(m_mutex);
//         m_value = 0;
//     }

// private:
//     mutable std::mutex m_mutex;
//     unsigned int m_value = 0;
// };

// void test_shared_mutex()
// {
//     shared_mutex_counter counter;
//     int temp;

//     // 写线程函数
//     auto writer = [&counter]() {
//         for (int i = 0; i < LOOP_COUNT; i++)
//             counter.increment();
//     };

//     // 读线程函数
//     auto reader = [&counter, &temp]() {
//         for (int i = 0; i < LOOP_COUNT; i++)
//             temp = counter.get();
//     };

//     // 存放读线程对象指针的数组
//     std::thread** tarray = new std::thread* [READER_THREAD_COUNT];
    
//     clock_t start = clock();
//     for (int i = 0; i < READER_THREAD_COUNT; i++)
//         tarray[i] = new std::thread(reader);

//     std::thread tw(writer);
//     for (int i = 0; i < READER_THREAD_COUNT; i++)
//         tarray[i]->join();
    
//     tw.join();

//     clock_t end = clock();
//     std::cout << "[test_shared_mutex]" << std::endl;
//     std::cout << "thread count: " << READER_THREAD_COUNT << std::endl;
//     std::cout << "result: " << counter.get() << ", cost: " << end - start << ", temp: " << temp << std::endl;
// }

// void test_mutex()
// {
//     mutex_counter counter;
//     int temp;
//     auto writer = [&counter]() {
//         for (int i = 0; i < LOOP_COUNT; i++)
//             counter.increment();
//     };

//     auto reader = [&counter, &temp]() {
//         for (int i = 0; i < LOOP_COUNT; i++)
//             temp = counter.get();
//     };

//     std::thread** tarray = new std::thread* [READER_THREAD_COUNT];
//     clock_t start = clock();
//     for (int i = 0; i < READER_THREAD_COUNT; i++)
//         tarray[i] = new std::thread(reader);

//     std::thread tw(writer);
//     for (int i = 0; i < READER_THREAD_COUNT; i++)
//         tarray[i]->join();

//     tw.join();
//     clock_t end = clock();
//     std::cout << "[test_mutex]" << std::endl;
//     std::cout << "thread count: " << READER_THREAD_COUNT << std::endl;
//     std::cout << "result: " << counter.get() << ", cost: " << end - start << ", temp: " << temp << std::endl;
// }

// int main()
// {
//     test_mutex();
//     // test_shared_mutex();
//     return 0;
// }

// // g++ -g -o thread_test thread_test.cpp -lpthread -std=c++17

/*
std::condition_variable
条件变量
*/

// #include <thread>
// #include <mutex>
// #include <condition_variable>
// #include <list>
// #include <iostream>

// class Task
// {
// public:
//     Task(int myTaskID): taskID(myTaskID){}

//     void doTask()
//     {
//         std::cout << "handle a task, taskID: " << taskID << ", threadID: " << pthread_self() << std::endl;
//     }

// private:
//     int taskID;
// };

// // pthread_mutex_t mymutex; linux平台代码
// std::mutex mymutex; // C++代码,可以同时兼容linux 和 windows
// std::list<Task*> tasks;
// // pthread_cond_t mycv; linux平台代码
// std::condition_variable mycv;

// void* consumer_thread()
// {
//     Task* pTask = NULL;
//     while (true)
//     {
//         // pthread_mutex_lock(&mymutex); 为了配合 pthread_cond_wait方法,需要提前加锁
//         // // 这里使用while的原因是可能会被虚假唤醒，因此当wait被唤醒之后，还需要判断tasks是否为空
//         // while (tasks.empty())
//         //     // 如果获得了互斥锁，但是条件不合适，则pthread_cond_wait 会释放锁，不往下执行，所以在这语句之前有lock的操作
//         //     // 发生变化后，如果条件合适，则pthread_cond_wait会阻塞到获得锁，所以在这语句之后有unlock的操作
//         //     pthread_cond_wait(&mycv, &mymutex);


//         // pTask = tasks.front();
//         // tasks.pop_front();

//         // pthread_mutex_unlock(&mymutex); 然后手动解锁

//         {
//             std::unique_lock<std::mutex> guard(mymutex); // c++的是使用RAII封装,因此使用时需要限定使用范围,以达到退出范围时自动释放的功能
//             while (tasks.empty())
//             {
//                 mycv.wait(guard);
//             }
//             pTask = tasks.front();
//             tasks.pop_front();
//         }
//         if (pTask == NULL)
//             continue;

//         pTask->doTask();
//         delete pTask;
//         pTask = NULL;
//     }

//     return NULL;
// }

// void* producer_thread()
// {
//     int taskID = 0;
//     Task* pTask = NULL;

//     while (true)
//     {
//         pTask = new Task(taskID);
//         // pthread_mutex_lock(&mymutex); // 这里加锁是为了操作tasks时不会有脏数据,和上面为了wait而加的锁有本质区别
//         // tasks.push_back(pTask);
//         // std::cout << "produce a task, taskID: " << taskID << ", threadID: " << pthread_self() << std::endl;
//         // pthread_mutex_unlock(&mymutex); 

//         // pthread_cond_signal(&mycv); // 发出信号
//         // taskID++;
//         // sleep(1); // 线程休眠

//         { // 使用{}减小作用范围
//             std::lock_guard<std::mutex> guard(mymutex); // 注意mymutex的生命周期要大于guard!!!!!!!!!!!!!!
//             tasks.push_back(pTask);
//             std::cout << "produce a task, taskID: " << taskID << ", threadID: " << pthread_self() << std::endl;
//         }
//         std::cout << "is empty = " << tasks.empty() << std::endl;
//         mycv.notify_one(); // 发出信号
//         taskID++;
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }
//     return NULL;
// }

// int main()
// {
//     // pthread_mutex_init(&mymutex, NULL); // 用以保证tasks列表操作的安全性
//     // pthread_cond_init(&mycv, NULL); // C++不用显式的初始化

//     // 五个消费线程
//     // pthread_t consumerThreadID[5];
//     std::thread consumerThreadID[5];
//     for (int i = 0; i < 5; i++)
//         consumerThreadID[i] = std::thread(consumer_thread);
//         // pthread_create(&consumerThreadID[i], NULL, consumer_thread, NULL);

//     // 一个生产者线程
//     // pthread_t producerThreadID;
//     // pthread_create(&producerThreadID, NULL, producer_thread, NULL);
//     std::thread producer(producer_thread);

//     producer.join();
    
//     // pthread_join(producerThreadID, NULL);

//     for (int i = 0; i < 5; ++i)
//         consumerThreadID[i].join();
//         // pthread_join(consumerThreadID[i], NULL);

//     // pthread_cond_destroy(&mycv);
//     // pthread_mutex_destroy(&mymutex); C++不用显式销毁


//     return 0;
// }


/*
条件变量的应用:确保线程启动成功
*/

// #include <thread>
// #include <mutex>
// #include <condition_variable>
// #include <iostream>
// #include <vector>
// #include <memory>

// std::mutex mymutex;
// std::condition_variable mycv;
// bool success = false;


// void thread_func(int no)
// {
//     {
//         std::unique_lock<std::mutex> lock(mymutex); 
//         success = true; // 逻辑跑到这里说明线程已经被创建成功
//         mycv.notify_all();
//     }
//     std::cout << "thread create success, thread no: " << no << std::endl;
//     while (true); // 简单死循环模拟
// }

// int main()
// {
//     std::vector<std::shared_ptr<std::thread>> threads;
//     for (int i = 0; i < 5; ++i)
//     {
//         success = false;
//         // std::shared_ptr<std::thread> spthread;
//         // spthread.reset(new std::thread(thread_func, i));
//         auto spthread = std::make_shared<std::thread>(thread_func, i);
//         {
//             std::unique_lock<std::mutex> lock(mymutex);
//             while (!success)
//                 mycv.wait(lock); // 虽然这里在未接收到信号时会释放锁,但是该锁并不会被本线程获取到,因此本线程会一直阻塞在这里
//         }
//         std::cout << "start thread success: thread no: " << i << std::endl;
//         threads.emplace_back(spthread);
//     }

//     for (auto& iter: threads)
//         iter->join();

//     return 0;
// }

/*
线程局部存储
linux:
1. 
int pthread_key_create(pthread_key_t* key, void (*destructor)(void*)); 
destructor是自定义函数指针,函数签名如下:
void* destructor(void* value)
{
    // .. 当线程终止时,如果key关联的值不为NULL,则会自动执行该函数,如果无需析构,那么将destructor参数设置为NULL即可
}


int pthread_key_delete(pthread_key_t key);

int pthread_setspecific(pthread_key_t key, const void* value);
void* pthread_getspecific(pthread_key_t key);
函数会调用 pthread_key_create 函数申请一个槽位,返回一个小于1024的无符号整数填入pthread_key_t中, 一共有1024个槽位,记录槽位分配情况的数据结构pthread_keys是进程唯一的


2. 线程局部变量
__thread int val = xxx; 

C++11:

thread_local int val = xxx;
*/
