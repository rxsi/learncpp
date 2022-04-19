/*
创建型:
单例模式
*/
#include <iostream>

class Singleton
{
public:
    static Singleton& getInstance()
    {
        return singleton;
    }
private: // 当然一种方式可以是把他们声明为delete
    Singleton();
    ~Singleton();
    Singleton (const Singleton&);
    Singleton& operator=(const Singleton&);

private:
    static Singleton singleton;
};

Singleton Singleton::singleton; // static变量会在main函数执行之前完成初始化.和全局变量一样


int main()
{
    Singleton& s1 = Singleton::getInstance();
    Singleton& s2 = Singleton::getInstance();
    // std::cout << &s1 << std::endl;
}