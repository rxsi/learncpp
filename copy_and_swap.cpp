/*
1. copy_and_swap的作用是简化代码,且保证赋值操作符异常时,不丢失原有数据

当设计一个类时,我们通常需要提供5个方法:
1). 默认构造函数
2). 拷贝构造函数(默认编译器合成的方法只能处理浅拷贝)
3). 拷贝赋值运算符
4). 移动构造函数
5). 移动赋值运算符

当设计拷贝赋值运算符时,需要处理的情况有:
a. 自赋值检测
b. 对数据进行拷贝

其中可发现b情况其实就是拷贝构造函数做的事,因此可以优化为调用拷贝构造函数



当我们在设计拷贝赋值运算符时,通常需要保证以下几点:
*/
// #include<iostream>
// using namespace std;

// class Base {
// private:
//     int size;
//     int *ptr;

// public:
//     Base(int size_num): size{size_num} {
//         cout << "Base(int size_num)" << endl;
//         ptr = new int[size];
//     }

//     Base(const Base& base): Base(base.size){ //可以通过调用构造函数的方式,减少拷贝构造函数的代码冗余
//         cout << "Base(const Base& base)" << endl;
//         for (int i = 0; i < base.size; i++){
//             ptr[i] = base.ptr[i];
//         }
//     }

//     // Base& operator=(const Base& base){ // 版本1: 没有处理delete之后,在new阶段失败而导致的原数据丢失问题
//     //     cout << "Base& operator=(const Base& base)" << endl;
//     //     if (this != &base){
//     //         delete[] ptr;
//     //         size = base.size;
//     //         ptr = new int[size];
//     //         for (int i = 0; i < size; i++){
//     //             ptr[i] = base.ptr[i];
//     //         }
//     //     }
//     //     return *this;
//     // }

//     Base& operator=(Base base){
//         cout << "Base& operator=(Base base)" << endl;
//         d 
//     }

//     Base(Base&& base){
//         cout << "Base(Base&& base)" << endl;
//         size = base.size;
//         ptr = base.ptr;
//         base.ptr = nullptr;
//     }

//     Base& operator=(Base&& base){
//         cout << "Base& operator=(Base&& base)" << endl;
//         if (this != &base){
//             size = base.size;
//             delete[] ptr;
//             ptr = base.ptr;
//             delete[] base.ptr;
//         }
//         return *this;
//     }
// };

// // g++ -fno-elide-constructors copy_and_swap.cpp -o copy_and_swap
// // 禁止编译器优化
// int main(){

// }

#include <iostream>
#include <string>
namespace A{

class ClassTest{
public:
    friend std::ostream& operator<<(std::ostream &os, const ClassTest& s);
    // friend void swap(ClassTest &a, ClassTest &b) noexcept;
    ClassTest(std::string s = "abc") :str(new std::string(s)){} //默认构造函数
    ClassTest(const ClassTest &ct) :str(new std::string(*ct.str)){} //拷贝构造函数
    ClassTest &operator=(const ClassTest &ct) //拷贝赋值函数
    {
        str = new std::string(*ct.str);
        return *this;
    }
    ~ClassTest() //析构函数
    {
        delete str;
    }
    void swap(ClassTest &t) noexcept
    {
        using std::swap;
        swap(str,t.str); //交换指针，而不是string数据
    }
private:
    std::string *str;  //一个指针资源
};
std::ostream& operator<<(std::ostream &os,const ClassTest& s)
{
    os << *s.str;
    return os;
}
void swap(ClassTest &a, ClassTest &b) noexcept
{
    std::cout << "use user swap" << std::endl;
    a.swap(b);
}
}

void swap(A::ClassTest &a, A::ClassTest &b) noexcept
{
    std::cout << "use global swap" << std::endl;
    a.swap(b);
}
int main(int argc, char const *argv[])
{
    A::ClassTest ct1("ct1");
    A::ClassTest ct2("ct2");
    std::cout << ct1 << std::endl;
    std::cout << ct2 << std::endl;
    using std::swap; // 如果这里加上了,那么可以顺利查找到user定义的,因为它屏蔽掉了全局作用域的(global). 如果注释掉,会同时查找到global和user的,从而编译错误
    swap(ct1, ct2);
    std::cout << ct1 << std::endl;
    std::cout << ct2 << std::endl;
    return 0;
}
/*
关于名字查找的一些问题(针对函数调用的情况):
首先，如果通常的无限定查找所生成的集合含有下列任何内容，那么不考虑实参依赖查找(ADL):
1) 类成员的声明
2) 块作用域的（非 using 声明的）函数声明
3) 任何非函数或函数模板的声明（例如函数对象或另一变量，其名字与正在查找的函数名冲突）

也就是说:当经过普通的名字查找后（没有包括ADL），如果候选函数中有类成员、块作用域中的函数声明（不包括using声明引入的）、其他同名的函数对象或变量名，则不启动ADL查找了。

当进入ADL查找时:
1. 不使用namespace A{}将ClassTest封装:
    1)当调用swap(ct1, ct2)时,会优先从当前作用域查找,即main作用域
    2)main作用域没有查找到swap函数,则向外层寻找,在全局作用域查找到了swap函数,因此停止查找.调用该函数

2. 使用namespace A{}将ClassTest封装:
    1)当调用swap(ct1, ct2)时,会优先从当前作用域查找,即main作用域
    2)main作用域没有查找到swap函数,则向外层寻找,即全局作用域
    3)全局作用域查找不到,又因为ct1是一个类变量,因此查找类的作用域,发现namespace A{}中定义有swap函数.因此调用
    4)ADL 能找到完全在类或类模板内定义的友元函数（典型为重载的运算符），即使它始终未在命名空间层次进行声明。(也就是说swap不是定义在namespace A中,则需要在类中声明为友元函数并定义)
*/