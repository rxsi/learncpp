#include<iostream>
#include<string.h>
#include<vector>
#include<typeinfo>
using namespace std;

/*
模板特例化、偏特例化、函数重载之间的关系
*/

// 特例化

// 函数特例化(函数不可以偏特化)
// 首先定义了一个通用的函数模板，用以返回二者之间的较大者
template <typename T> T Max(T t1, T t2)
{
    cout << "use normal template Max" << endl;
    return (t1 > t2) ? t1 : t2;
}

// 当我们需要对某些特殊类型进行处理时，比如比较字符串，我们希望使用strcmp进行比较，因此进行特例化处理
// 格式为template<>  和 函数后面加<类型>
template<> const char* Max<const char*>(const char* t1, const char* t2)
{
    cout << "use spe template Max" << endl;
    return (strcmp(t1, t2)) > 0 ? t1 : t2;
}

// 当我们使用了函数重载时，编译器会优先选择使用非模板的函数，因为它更加特例化
// 但是有两个问题：1）因为是非模板，因此不管是否有调用，编译阶段都会生成对应的二进制数据，而模板则是有用才生成;2）当处于分离编译模式下时，应该在各个源文件中都声明该函数，否则使用的将会是模板函数
// 注意，应该要避免模板函数和非模板函数的重载，而应该使用特例化，因为在不同的编译器下会有不同的规则导致bug。
const char* Max(const char* t1, const char* t2)
{
    cout << "use normal Max" << endl;
    return (strcmp(t1, t2)) > 0 ? t1 : t2;
}

// 模板重载
template<typename T> T Max(T* t1, T* t2){ 
    cout << "use overload template Max" << endl;
}

/* 模板函数与非模板函数重载的bug问题示例 */

template <typename T>
int compare2(const T& lhs, const T& rhs)
{
    std::cout << "template compare2" << std::endl;
    return 0;
}
 
int compare2(const char* lhs, const char* rhs)
{
    std::cout << "ordinary compare2" << std::endl;
    return 0;
}
 
int main(int argc, char *argv[])
{
    char c1[] = "hello";
    char c2[] = "hello";
    compare2(c1, c2);
}

// 在g++和clang编译下，具现化的结果为int compare2<char [6]>(char const (&) [6], char const (&) [6])，因此调用的是模板函数
// 而在vs编译下，则是将字符串转成了指针，即匹配了ordinary的那个，因此出现不同编译器下有不同的运行结果
// 当然如果两个字符串c1和c2的长度不同的话，那么在g++和clang编译器下匹配的还是ordinary版本
/* =========================================================== */

// 类特例化
// 首先定义一个通用的类模板
template <typename T> class Base
{
public:
    Base()
    {
        cout << "create normal template Base" << endl;
    }
};

template <> class Base<char*>
{
public:
    Base()
    {
        cout << "create spe template Base" << endl;
    }
};

// 偏特例化
// 主要分为两种：1）对部分模板参数进行全特化; 2）对模板参数特性进行特例化，包括将参数特例化为指针、引用、另一个模板类等
// 普通通用函数模板
template <typename T1, typename T2> void compare(T1 t1, T2 t2)
{
    cout << "use normal template compare " << "t1: " << typeid(t1).name() << " " << t1 << " t2: " << typeid(t2).name() << " " << t2 << endl;
}
// typeid判断指针类型时得到的是pointer类型，而判断引用类型时，得到的是引用指向对象的类型

// 部分参数特例化
template <typename T2> void compare(int t1, T2 t2)
{
    cout << "use spe int template compare" << endl;
}

// 特例化参数类型
template <typename T1, typename T2> void compare(T1* t1, T2* t2)
{
    cout << "use spe pointer template compare" << endl;
}

// 特例化参数类型
// std::swap 就有一个特例化的版本，接收的是数组类型的参数，因为数组比较特殊不能单纯的交换（时间复杂度O(1)），而是需要遍历的方式进行修改（时间复杂O(n)）
template <typename T1, typename T2> void compare(vector<T1>& vecLeft, vector<T2>& vecRight)
{
    cout << "use spe vector template compare" << endl;
}

// 类模板特例化与函数模板特例化相似

int main()
{
    Max(1, 2); // 使用普通版本
    Max("1", "2"); // 使用特例化版本，如果存在重载的普通函数，则优先使用普通函数
    int *ptr_a = new int(1);
    int *ptr_b = new int(2);
    Max(ptr_a, ptr_b); // 使用重载的指针版本

    Base<int>(); // 通用版本
    Base<char*>(); // char*版本

    compare(30, 31); // 这种调用方式，编译器会发现normal和spe int 都匹配上，但是spe int更加特例，因此会调用spe int
    compare<double, int>(31, 31); // 这种调用方式，编译器发现只有normal的两个参数的匹配上，因此会调用normal
    // 如果只有一个模板函数，那么compare(30, 31)属于让编译器自己去根据参数识别出T的类型，而compare<int, int>(30, 31)则是让显式的告知编译器参数类型
    // 且可以指定参数类型，从而达到参数类型的转换，如 compare<double, int>(30, 31); 将30转换为double类型 

    int a = 30;
    int b = 31;
    compare(&a, &b); // 使用指针版本，如果不定义指针版本，那么会是使用normal版本，但是通用的compare规则并不适用于指针，因此我们需要特例化指针版本

    vector<int> vecLeft{0};
    vector<int> vecRight{1,2,3};
    compare(vecLeft,vecRight); // 同样道理，需要对vector类型进行特殊的处理。因此定义了特例化的函数
}