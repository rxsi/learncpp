#include <iostream>
#include <thread>

int global_g = 1;

void thread_func()
{
    std::cout << "thread ID: " << std::this_thread::get_id() << std::endl;
    std::cout << "global_g address: " << &global_g << " value: " << global_g << std::endl;
    global_g++;
    return;
}

int main()
{
    std::thread t1(thread_func);
    std::thread t2(thread_func);
    t1.join();
    t2.join();
}