/* 有循环依赖问题 */
// #ifndef PRE_1H
// #define PRE_1H

// #include "pre_2.h"
// #include <vector>
// #include <string>

// class A
// {
//     typedef std::vector<std::string>::size_type st;
//     B b;
// };

// #endif


/* 前置声明,同时改为使用 B* 指针*/
#ifndef PRE_1H
#define PRE_1H

#include <vector>
#include <string>

class B;

class A
{
public:
    typedef std::vector<std::string>::size_type st;
    B* b;

    void fun();
};

#endif
/*
为了解决循环依赖问题，因此 pre_1.h 不能include pre_2.h 头文件
而又需要使用B，因此使用前置声明

因为使用了前置声明，因此编译器无法确定对象的实际大小，因此使用指针
指针的大小在机器类型上是确定的，因此可以编译通过（32为4字节，64为8字节）

*/