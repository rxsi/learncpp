#ifndef PRE_2H
#define PRE_2H

#include "pre_1.h"
#include <iostream>

class B
{
public:
    A a;
    A::st num;

    void doSomething()
    {
        std::cout << "do something" << std::endl;
    }
};

#endif
/*
以上面的方式编译,会报错
include/pre_2.h:8:5: error: ‘A’ does not name a type
     A a;

原因是pre_1.h 和 pre_2.h 形成了循环依赖问题
解决方式有:
1) 结构优化,比如设计一个class IB,一个抽象类,class B是它的实现之一.这样class A 和 class B就都依赖于class IB,就不会形成循环依赖了
2) 前置声明

前置声明的优点：
1）因为避免了直接导入整个头文件，使得编译时无需展开过多的文件，编译速度更快
2）避免了头文件的小改动都需要重新编译整个文件

缺点：
1）前置声明不能处理typedef声明的strcut类，如
// pre_1.h
typedef struct Student
{
    int a = 10;
} STUDENT;

// pre_2.h
// struct STUDENT; // 编译报错
struct Student; // 这种方式就可以

2）当使用前置声明 + 类指针后，不能在当前头文件中使用该指针，因此我们没有导入对应的头文件，只能创建
对应源文件然后在该源文件中同时include两个头文件，再实现

3）使用前置声明之后，如果该类要改名字或者移动命名空间，则会相当麻烦

4）对于模板类，using，默认值都存在一定的问题

5）对于有继承关系的类，会造成异常
// a.h
class B {};
class C : B {};

// b.h
#include "a.h"
void f(B*)
void f(void*)
void test(C* x) {f(x)} // 因为使用的是include方式，调用的是 f(B*)，但是如果使用前置声明，则会调用 f(void*)


*/
