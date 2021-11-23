#include <iostream>
using namespace std;
// class Base
// {
// public:
//     Base(int i): mem(i)
//     {
//         cout << "init Base " << i << endl;
//     }

//     int get_men()
//     {
//         return mem;
//     }

// protected:
//     int mem;
// };

// class Derived: Base
// {
// public:
//     Derived(int i): mem(i), Base(i) // 如果父类的构造函数有参数，子类需要主动调用，否则可以忽略，会默认调用无参的构造函数
//     {
//         cout << "init Derived " << i << endl;
//     }

//     int get_mem(int i)
//     {
//         return i;
//     }

// protected:
//     int mem;
// };



// int main()
// {
//     Derived d(10); // 使用new创建的对象会在堆上，返回的是指针，需要手动调用delete释放，使用智能指针可以不用手动释放，一般用来作共享传递时才是用new的方式，因为可以避免重复创建。直接初始化则是在栈上，无需delete
//     //d.get_mem(); // 当子类构造了与父类同名的函数时，会隐藏掉父类的函数，不管参数是否一致。
// }

/*

*/
class CB
{
public:
    virtual void f()
    {
        cout << "CB::f()" << endl;
    }

    void t()
    {
        cout << "CB::t()" << endl;
    }
};


class CD : public CB
{
public:
    void f() override
    {
        cout << "CD::f()" << endl;
    }

    void t()
    {
        cout << "CD::t()" << endl;
    }
};

class CE: public CD
{
public:
    // void f() override // 对CB父类的虚函数进行覆盖
    // {
    //     cout << "CE::fun()" << endl;
    // }

    void t()
    {
        cout << "CE::t()" << endl;
    }
};

int main(int argc, char* argv[])
{
    // 如果子类CD没有覆盖父类CB中的虚函数f(),那么p1->f()将会调用到父类的虚函数方法
    // 在CD中f()是被隐藏了
    CD *p1 = new CE();
    p1->f();

    // CD *p2 = new CD();
    // p2->f();
    // CD cd;
    // cd.f();

    // CD *p2 = new CD(); 
    // p2->f();
}