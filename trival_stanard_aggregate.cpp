#include<memory>
#include<iostream>
#include <type_traits>
#include <new>
#include <utility>

// class A{ // 这种情况下,符合trivial的定义,但是定义了多个的访问控制符,而C本身不支持,因此不是standard-layout,也就不是pod类型了
// public:
//     int a;
// private:
//     int b;
// public:
//     int c;
// };

// class B{
// public:
//     int a;
//     int b;
// private:
//     static int c;
// };



// int main(){
//     // std::cout << std::is_pod<A>::value << std::endl; // 0
//     // std::cout << std::is_trivial<A>::value << std::endl; // 1
//     // std::cout << std::is_standard_layout<A>::value << std::endl; // 0

//     std::cout << std::is_pod<B>::value << std::endl; // 1
//     std::cout << std::is_trivial<B>::value << std::endl; // 1
//     std::cout << std::is_standard_layout<B>::value << std::endl; // 1
// }


/*
https://andreasfertig.blog/2021/01/cpp20-aggregate-pod-trivial-type-standard-layout-class-what-is-what/
trival:最主要是支持静态构造
拥有连续的内存空间,所以可以使用memcopy进行直接的内存复制,支持静态构造
直接copy到一个char大小的字节或者unsigned char字节.(一个空类大小为一字节)
没有定义自己的构造函数(默认构造,拷贝构造,析构,拷贝赋值)或者 定义的构造函数是使用=default声明的, 除此之外的定义了自己的构造函数仍是trivial的???
允许有多个访问控制符,比如public\private\protected等,

standard-layout:最主要是和C的内存布局一致
内存布局和C一致,不允许出现C++特有特性,比如访问控制符
1)没有虚继承
2)没有虚函数
3)没有引用成员变量
4)所有的非静态成员变量有相同的访问控制符(也就是说静态成员变量定义在哪个访问控制符都ok. 如示例B)
5)若有父类,则父类都是standard-layout类型的
6)所有的非静态成员变量属性都是standard-layout的(反例:有一个类是非standard-layout的,然后在本类中有他的一个实例)

*/
// standard-layout的反例:
// struct Base {};

// struct NotStandardLayoutBase {
//   int& ref;  //3 
// };

// struct NotStandardLayout : virtual Base,                   //1 
//                            public NotStandardLayoutBase {  //5 

//   virtual void Fun();  //2 Virtual function

// private:
//   int                   a;  //4 Mixing of access specifiers
//   NotStandardLayoutBase b;  //6 A non-standard-layout member
// };

/*
pod:同时满足trivial(静态构造)和standard-layout(内存布局和C一致)
C++20之后移除了std::is_pod<>()判断方法,直接使用std::is_trival 和 std::is_standard_layout 组合判断即可
满足pod的C++程序可以在C程序中运行(这应该是最大的用途了)

aggregate:满足条件的可以直接使用{}进行初始化.不知道有什么引用场景
array天然就是aggregate类型的, 即可以使用 int [3] = {1, 2, 3} 进行初始化
某个类如果也是aggregate类型的, 那么可以SomeType a{2, 3, 4} 进行初始化
*/

// Base is no aggregate due to the deleted default constructor
struct Base
{
  int i;
  Base(int a)
  : i(a)
  {}
};

// A is no aggregate as well
struct A
{
  int i;
  A(int a)
  : i(a)
  {}
};

// SomeType is an aggregate
struct SomeType // :Base 如果是C++20,则可以给类添加非aggregate的父类
{
  int x;
  A   y;
};



class Test{
public:
    Test(){}
    Test(int a):i(a){}
    Test(const Test& t): Test(t.i){}
    Test& operator=(const Test& t){
        i = t.i;
        return *this;
    }

private:
    int i;
};


class MyType{
public:
    MyType(std::initializer_list<int> list) {
        for (auto iter = list.begin(); iter != list.end(); ++iter){
            if (iter != list.end()){
                Test t(*iter);
                y = t;
            }else{
                x = *iter;
            }
            // std::cout << *iter << std::endl;
            // x(*iter);
            // y(*iter);
        }
    }

private:
    int x;
    Test y;
};

int main(){

    SomeType a{2, 3}; // 如果C++20以后且有父类Base, 则 SomeType a{2, 3, 4} 等价于 Base is 2, x=3 and y=4
    std::cout << a.x << " " << a.y.i << std::endl; // 输出2, 3

    // static_assert(not std::is_aggregate_v<Base>);
    // static_assert(not std::is_aggregate_v<A>);

    MyType b{2, 3}; // 这个效果看起来和SomeType一样,实际上运用的是std::initial_list实现的,有本质区别
}
