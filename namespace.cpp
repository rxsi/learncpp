#include<iostream>
#include<hashtable.h>
/*
命名空间

1. 命名空间可以是不连续的，即可以在不同的文件中定义相同的命名空间名，会自动进行连接

2. 当我们在命名空间内声明，而在命名空间外定义，则需要加上命名空间的前缀名

*/

// 3. 模板特例化必须定义在原始模板所属的命名空间，或者声明在命名空间中，而在命名空间外定义
class Sales_data {};
namespace std
{
    template <> struct hash<Sales_data>;
}

// 4. 全局命名空间是隐式声明的
:: Sales_data; // 使用::访问全局命名空间定义的成员

// 5. 嵌套的命名空间，内层声明会隐藏外层的同名声明

// 6. 内联命名空间，可以使外层直接调用空间内的名字
// 常用来作版本迭代区分
inline namespace FifthEd
{
    class Query_base {};
}

namespace FourthEd
{
    class Query_base {};
}

namespace cplusplus_primer
{
    // #include "FifthEd.h"
    // #include "FourthEd.h"
    // 假设分别定义在了两个不同的头文件，将头文件引入命名空间之后将会使头文件中的所有成员都成为命名空间内的成员
    // FifthEd是内联的，因此调用cplusplus_primer::Query_base得到的是fifthEd版本的，
    // cplusplus_primer::FourthEd::Query_base调用的将会是FourthEd版本
}

// 7. 未命名的命名空间，等效于C中的static的作用，可以使变量拥有静态的生命周期，且只能在本文件中生效，即不会有多定义的问题
// 未命名的命名空间内部的成员可以直接使用，与外层同级，因此要注意命名冲突的问题
int i;
namespace
{
    int i; // 同名冲突了
}

// 8. using 声明 和 using 指示
// using声明
using std::abort; // 导入的成员的作用域与当前作用域一致，因此不能有命名冲突（同名同参）
// using指示
using namespace std; // 导入的成员的作用域与当前作用域的上层一致，且尽管上层有命名相同的成员（同名同参），只是产生了二义性，可以使用不同的命名空间前缀访问(std::)
// 如果当前作用域有命名相同的成员，则会屏蔽掉新引入的成员

namespace blip
{
    int a = 16, b = 15, c = 23;
}
int b = 0;
void mainip()
{
    using namespace blip;
    ++a; // 修改的是blip::a
    //++b; 报错，因为没有声明使用的是哪个命名空间的成员 
    ++::b; // 全局作用域的
    ++blip::b; // blip作用域的
    int c = 98; 
    ++c; // 局部作用域的
}

// 9. 当函数参数函数有类、类引用、类指针时，如果在常规的命名空间中查找不到函数，则会从该类所属的命名空间中查找。