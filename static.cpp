/*
静态成员
不与具体的对象关联，只与类相关，存放的位置是在全局变量区
而且不占类的空间，比如用sizeof(A)不会计算static变量的容量
static可以限定使用的作用域
1）在函数内声明，只会被分配一次，作用域是该函数内
2）在模块中声明{},则作用域是在模块内
3）在类内static成员变量，作用域是类
4）在类内static成员函数，作用域是类
由此可见，不能在以上其他地方使用static，因为会使得整个static对整个文件可见，而当某个文件导入了该文件后也就可以访问了，破坏了作用域性质。


1.静态成员函数
a. 不能声明为const函数，因为编译器会为const添加一个隐式的const this指针，类似于python的self，而static是没有this指针的
b. 可以在类内定义，也可以在类内声明，而在类外定义，但是在类外定义时不能加static，只能在声明的语句加static
原因：当允许在百纳量

2. 静态成员变量
a. non-const变量只能是在类内声明，而在类外定义。原因是如果允许在类内定义，那么当某个文件导入该文件（假设是头文件）那么就会存在多个A::i，这破坏了ODR规则
b. const变量可以直接在类内定义

3. 改变变量的链接性
当一个变量被static修饰时，他的作用域只会在当前文件（C的规则）
在C++中使用匿名命名空间实现这一效果
因为C++只能是一次定义，因此如果不同文件定义了相同的变量名，将会冲突，使用匿名命名空间即可规避掉这一问题
*/

// #include<iostream>
// using namespace std;
// class A
// {
// public:
//     int k; // 如果没有初始化,是个随机值
//     static void test();
//     static const void test1(){};
//     static int j; // 如果没有在类外显式的定义,那么会编译不过.
// private:
//     static int i; 
//     static const int period = 30; // 可以在类内直接定义
//     static constexpr int grade = 40; // 可以在类内直接定义，使用constexpr编译器会进行校验。
// };

// void A::test(){} // 不能加static关键字

// int A::i = 10;

// int m; // 全局变量和静态变量会被存储在BSS段(一个默认全为0的内存空间),因此初始值为0
// static int n; //初始值为0
// int main()
// {
//     A a;
//     cout << a.k << endl;
//     int l;
//     cout << l << endl;
//     cout << m << endl;
//     cout << n << endl;
// }

// #include<iostream> 
// using namespace std; 

// class Apple 
// { 
//     int i; 
//     public:
//         Apple() 
//         { 
//             i = 0; 
//             cout << "Inside Constructor\n"; 
//         } 
//         ~Apple() 
//         { 
//             cout << "Inside Destructor\n"; 
//         } 
// }; 

// int main() 
// { 
//     int x = 0; 
//     if (x==0) 
//     { 
//         Apple obj; // C++ 中在if等定义的变量出了这个作用域后就会销毁,这与Python不同!!!!!!!!!    
//     } // 当Apple对象不是静态时,if调用结束立即就会调用析构函数.
//     // 当使用 static Apple obj; 则会在整个程序结束后再析构
//     cout << "End of main\n"; 
// } 

// static不能声明为virtual, const
// 不能是virtual的原因: 因为virtual需要this指针调用,即__thiscall.而静态函数没有this指针,
// 而且静态成员函数是在编译期就确定的,而virtual是在运行时绑定的

// 不能是const的原因: 因为const需要this指针调用,本质上,this指针是指针常量,当函数是const时,则实际上作用在this指针上,
// 变成了指向常量的指针常量.

// 同一个编译单元内，按照静态变量的构造顺序进行逆序析构
// 但是在不同的编译单元内，顺序是不确定的
// 因此可能出现A文件的静态方法调用了B文件中还未初始化的静态变量，
// 解决方式是把B中的静态变量变成local static，这样做可以保证在调用时（如果未初始化就初始化）就已经是初始化好的了，且线程安全，是实现单例模式的首选项


#include <iostream>
#include <memory>
#include <string>

class A
{
public:

    static const int a = 1;
    // static const float b = 2;
    static constexpr int c = 3;
    double d[a];
    float e[c];
    A()
    {
        std::cout << "construct A" << std::endl;
    }

    ~A()
    {
        std::cout << "destruct A" << std::endl;
    }
};

class B
{
public:
    B()
    {
        std::cout << "construct B" << std::endl;
        // 方式4:
        // m_count++;
    }

    ~B()
    {
        std::cout << "destruct B" << std::endl;
        // m_instance->~A(); // 不能单存的直接调用A的析构函数,因为实际上

        // 方式4:
        // m_count--;
        // if (m_count == 0)
        // {
        //     delete m_instance;
        //     m_instance = nullptr;
        // }
    }

private:
    // static A m_instance; 方式1:静态对象会在main函数退出之后被自动析构(不管析构函数是否是private)
    // static A* m_instance; 方式2:如果是静态指针,那么不会被自动析构,会造成内存泄漏
    // static std::shared_ptr<A> m_instance; // 方式3:使用智能指针,可以正确析构

    // 方式4: 使用计数,每创建一个新对象就+1,析构一个对象就-1,当为0时析构掉A对象
    // static int m_count;
    // static A* m_instance;
};

// A B::m_instance = A(); 方式1:
// A* B::m_instance = new A(); 方式2:
// std::shared_ptr<A> B::m_instance = std::make_shared<A>();方式3:

// 方式4:
// int B::m_count = 0;
// A* B::m_instance = new A();

// 方式5,使用RAII规则,将静态指针变为B中某个静态内部类的属性,然后当这个静态内部类被析构时,在析构函数中析构掉A对象

int main()
{
    B b1;
    B b2;
    B b3;
    return 0;
}