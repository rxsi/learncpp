#ifndef _FUNC_H
#define _FUNC_H

template <typename T> void Swap(T &a, T &b); // 声明了
void bubble_sort(int arr[], int n);

#endif


/*
对于函数，我们一般把函数的声明放在头文件中(.h)，而把具体的定义放在源文件中(.cpp)
对于类，我们一般把类的声明，即类内函数只是进行声明而非定义，放在头文件中(.h)，把成员函数的定义放在源文件中(.cpp)
编译时，只要某个文件有对头文件进行#include，则可获取到对应函数/类的声明，编译器就可以知道当前源文件中的函数调用是否正确;
而将函数调用和函数定义对应起来的过程，是放在了链接阶段。
因此有了链接器，我们才能够实现声明和定义的分离。

fdsf 

对于模板函数/类，不是真正的函数/类，需要把声明和定义都放在头文件中。特性：
1. 模板的实例化是按需进行的，用到哪个类型就初始化哪个类型
2. 模板的实例化是由编译器完成的，而不是链接器
3. 实例化的过程需要能够得知完整的定义

以本结构为例：
include/func.h
src/func.cpp
main.cpp

如果把模板函数/类的声明和定义都放在func.h头文件中，那么任何地方只要#include该头文件，就可以调用。

如果没有把声明和定义写在一起，则需要使用显式实例化，如 func.cpp 中的 template void Swap(double &a, double &b); 然后在调用处（main.cpp）中实例化声明 extern template void Swap(double &a, double &b);（其实也可以不用，编译器会自动寻找）
但是这样做有个弊端，当我们使用了新的类型的时候，都需要添加新的显式实例化。

所以最好的做法还是把声明和定义都写在头文件中。


多文件编译方式：
g++ -Iinclude src/func.cpp main.cpp -o main

-Ipath : 指定头文件路径
*/