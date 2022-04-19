/*
单例模式的实现
*/

// 1. 懒汉版本,即只有当第一次调用时才创建单例
/*
存在问题:
1. 内存泄漏, 虽然程序执行结束之后会自动清楚所申请的内存,但是并不会调用析构函数.有时候我们需要在析构函数做一些日志\io\db等的操作,因此调用析构函数是必要的.
根据单例模式的定义,我们不能提供外部可以直接delete的操作,即不能把析构函数定义为public.
2. 线程不安全


解决方法有:
1. 在类定义内部使用智能指针
2. 解决线程安全可以加锁
*/

// #include<iostream>
// #include<memory>
// using namespace std;
// class Singleton{
// private:
//     static Singleton* instance; // 所有对象共享,以实现只会有一个instance变量,从而可以利用instance == nullptr进行判断

// private: // 私有的构造函数和析构函数,使得外部无法直接实例化,只能调用getInstance方法去进行实例化
//     Singleton(){
//         cout << "Singleton()" << endl;
//     }

//     ~Singleton(){
//         cout << "~Singleton()" << endl;
//     } // 设置为private,以防止外部调用.当外部delete instance时编译器会显式报错.
//     // 如果定义成public, 外部可以delete instance,不符合单例模式的设计
// public:

//     Singleton(const Singleton&) = delete; // 可以定义为 Singleton(const Singleton&){}并放在private中.但是如果定义为delete,则应该放在public中,编译器可以更好的检测到.
//     Singleton& operator=(const Singleton&) = delete;

//     static Singleton* getInstance(){ // 设计成静态函数,使得可以直接通过类名调用,因为不可以在外部实例化,因此通过此方式完成实例化
//         if (instance == nullptr){
//             instance = new Singleton();
//         }
//         return instance;
//     }
// };

// Singleton* Singleton::instance = nullptr;


// int main(){
//     Singleton *instance = Singleton::getInstance();
//     Singleton *instance2 = Singleton::getInstance();
//     return 0;
// }


// 2. 线程安全+智能指针 的懒汉模式
// #include<iostream>
// #include<memory>
// #include<mutex>

// using namespace std;
// class Singleton{
// private:
//     static shared_ptr<Singleton> instance_ptr;
//     static mutex m_mutex;

// private:
//     Singleton(){
//         cout << "Singleton()" << endl;
//     }


// public:
//     ~Singleton(){ //因为使用的是智能指针,即需要delete,因此还是需要将析构函数定义为public,这并不好.
//         cout << "~Singleton()" << endl;
//     }
//     Singleton(const Singleton&) = delete;
//     Singleton& operator=(const Singleton&) = delete;

//     static shared_ptr<Singleton> getInstance(){
//         // lock_guard<mutex> lk(m_mutex); // 方式1:直接加锁,缺点:每次调用都会加锁,成本过高,因此引入双检锁.
//         // if (instance_ptr == nullptr){
//         //     instance_ptr = shared_ptr<Singleton>(new Singleton);
//         // }

//         // if (instance_ptr == nullptr){
//         //     lock_guard<mutex> lk(m_mutex); // 注意不可以写成这样,这样当多个线程都同时通过instance_ptr == nullptr时,虽然会在这里卡住,但是当获得锁之后还是都会执行后面的语句,造成实例化多个
//         //     instance_ptr = shared_ptr<Singleton>(new Singleton);
//         // }

//         if (instance_ptr == nullptr){ 
//             // 双检锁,但是会有可能失效.
//             // 考虑 shared_ptr<Singleton>(new Singleton) 的背后逻辑:
//             // 1) 申请内存
//             // 2) 在这个内存上构造对象
//             // 3) 将instance_ptr绑定到对象上
//             // 编译器并不保证2),3)的顺序.
//             // 因此当发生指令重排.如线程1跑到了b的时候,编译器先为instance_ptr绑定了对象地址,但是还没有执行对象的构造.此时线程2在这里判断非nullptr,然后执行了return.就会出现异常.

//             // 需要使用内存屏障 https://www.drdobbs.com/cpp/c-and-the-perils-of-double-checked-locki/184405726
//             // https://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/
//             lock_guard<mutex> lk(m_mutex); // 优化的方式是加入内存屏障.java的做法是使用volatite,C++则是使用atmoic库之类的,具体看上面链接.
//             if (instance_ptr == nullptr){ 
//                 instance_ptr = shared_ptr<Singleton>(new Singleton); //b
//             }
//         }
//         return instance_ptr;
//     }
// };

// shared_ptr<Singleton> Singleton::instance_ptr = nullptr;
// mutex Singleton::m_mutex;

// int main(){
//     shared_ptr<Singleton> instance = Singleton::getInstance();
//     shared_ptr<Singleton> instance2 = Singleton::getInstance();
//     return 0;
// }


// 3. 利用 local static 进行处理的懒汉版本(最推荐)
// C++ 不保证non-local static 的初始化顺序, 因此如果存在 A 静态变量去调用B静态变量的话,可能会出现异常
// 将non-local static 转换为local static即可保证当调用时,被调用方会完成初始化
// 且是线程安全的,因此可以用来实现单例模式(C++11之后):
// If control enters the declaration concurrently while the variable is being initialized, 
// the concurrent execution shall wait for completion of the initialization.

// #include <iostream>

// class Singleton
// {
// public:

//     Singleton(const Singleton&)=delete;
//     Singleton& operator=(const Singleton&)=delete;
//     static Singleton& get_instance(){
//         static Singleton instance;
//         return instance;

//     }
// private:
//     Singleton(){
//         std::cout<<"constructor called!"<<std::endl;
//     }

//     ~Singleton(){ // 静态变量存放在全局/静态数据区,当程序退出时,会逐个进行析构.且无论析构函数是否是private
//         std::cout<<"destructor called!"<<std::endl;
//     }
// };

// int main(int argc, char *argv[])
// {
//     Singleton& instance_1 = Singleton::get_instance();
//     Singleton& instance_2 = Singleton::get_instance();
//     return 0;
// }

// 4. 定义通用模板
// CRTP 奇异递归模板,即派生类作为基类的模板参数

// brief: a singleton base class offering an easy way to create singleton
// #include <iostream>

// template<typename T>
// class Singleton{
// public:
//     static T& get_instance(){
//         static T instance; // 本质上还是使用局部静态变量去实现线程安全
//         return instance;
//     }
//     virtual ~Singleton(){
//         std::cout<<"destructor called!"<<std::endl;
//     }
//     Singleton(const Singleton&)=delete;
//     Singleton& operator =(const Singleton&)=delete;
// protected:
//     Singleton(){
//         std::cout<<"constructor called!"<<std::endl;
//     }

// };
// /********************************************/
// // Example:
// // 1.friend class declaration is requiered!
// // 2.constructor should be private


// class DerivedSingle:public Singleton<DerivedSingle>{
//    // !!!! attention!!!
//    // needs to be friend in order to
//    // access the private constructor/destructor
//    friend class Singleton<DerivedSingle>; // 一定要定义为友元,因为需要在模板中调用T instance,即派生类的私有构造函数
// public:
//    DerivedSingle(const DerivedSingle&)=delete;
//    DerivedSingle& operator =(const DerivedSingle&)= delete;
// private:
//    DerivedSingle()=default;
// };

// int main(int argc, char* argv[]){
//     DerivedSingle& instance1 = DerivedSingle::get_instance();
//     DerivedSingle& instance2 = DerivedSingle::get_instance();
//     return 0;
// }

class Singleton
{
private:
    static Singleton instance;

    Singleton();
    ~Singleton();
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);

public:

    static Singleton& getInstance()
    {
        return instance;
    }
};

// initialize defaultly
Singleton Singleton::instance; // 会在main函数之前初始化,所以没有线程竞争问题
// 全局变量会在main函数开始之前初始化!!!!!