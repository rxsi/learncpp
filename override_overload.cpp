#include <iostream>
using namespace std;

/*
使用using实现对父类多个重载函数的重写
当一个子类只想要隐藏某个函数，而这个函数在父类有多个同名的重载函数，那么如果不全部进行隐藏，就会造成无法访问父类的同名函数。
当然如果一个都不隐藏，那么就都可以访问。
使用using声明，可以避免全部隐藏
*/

class A
{
public:
    void fun()
    {
        cout << "A::fun()" << endl;
    }

    void fun(int)
    {
        cout << "A::fun(int)" << endl;
    }

    void fun(float)
    {
        cout << "A::fun(float)" << endl;
    }
};

class B: public A
{
public:
    using A::fun; // 声明使用A作用域内的fun，然后就可以针对特定的目标函数进行隐藏
    void fun()
    {
        cout << "B::fun()" << endl;
    }

};

int main(){
    B b; // 调用类的无构造函数时，不能是B b()，因为会被认定为是定义了一个函数
    b.fun(); // B::fun()
    b.fun(1); // A::fun(int)
    b.fun(1.0f); // A::fun(float)
}