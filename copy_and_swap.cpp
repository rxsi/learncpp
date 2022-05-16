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

//     Base& operator=(const Base& base){ // 版本2:先确保能够成功创建新数据,再释放旧数据,这样确保了安全性
//         cout << "Base& operator=(const Base& base)" << endl; 
//         if (this != &base){
//             int newSize = base.size;
//             int *newPtr = new int[size];
//             for (int i = 0; i < newSize; ++i)
//             {
//                 newPtr = base.ptr[i];
//             }
//             delete[] ptr;
//             size = newSize;
//             ptr = newPtr;
//         }
//         return *this;
//     }

//     Base(Base&& base){
//         cout << "Base(Base&& base)" << endl;
//         size = base.size;
//         ptr = base.ptr;
//         base.ptr = nullptr;
//     }

//     Base& operator=(Base&& base){ // 兼容右值的情况
//         cout << "Base& operator=(Base&& base)" << endl;
//         if (this != &base){
//             size = base.size;
//             delete[] ptr;
//             ptr = base.ptr;
//             delete[] base.ptr;
//         }
//         return *this;
//     }

//     Base& operator=(Base base){...} // 如果使用拷贝的形式,那么可以省略拷贝赋值和移动赋值,但是性能不好.
// };

// // g++ -fno-elide-constructors copy_and_swap.cpp -o copy_and_swap
// // 禁止编译器优化
// 用上面的实现分别需要拷贝赋值和移动赋值,比较繁琐,并且要时刻注意自赋值处理和异常处理

#include <iostream>
#include <string>

namespace A{

int a = 1, b = 2;
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

void swap(int a, int b)
{
    std::cout << "use user swap" << std::endl;
}

} 

void Fun(int num)
{
    std::cout << "global Fun(int num)" << std::endl;
}

namespace B
{
    int i = 10;
    void Fun(int num)
    {
        std::cout << "B::Fun(int num)" << std::endl;
    }

    void swap(int a, int b)
    {
        std::cout << "use B::swap" << std::endl;
    }   
}

void swap(A::ClassTest &a, A::ClassTest &b) noexcept
{
    std::cout << "use global swap" << std::endl;
    a.swap(b);
}

void swap(int a, int b) noexcept
{
    std::cout << "use global swap" << std::endl;
}


int main(int argc, char const *argv[])
{
    // using B::swap;
    // A::ClassTest ct1("ct1");
    // A::ClassTest ct2("ct2");
    // std::cout << ct1 << std::endl;
    // std::cout << ct2 << std::endl;
    // int a = 1, b = 2;
    using std::swap; // 如果这里加上了,那么可以顺利查找到user定义的,因为它屏蔽掉了全局作用域的(global). 如果注释掉,会同时查找到global和user的,从而编译错误
    swap(A::a, A::b); 
    // std::cout << ct1 << std::endl;
    // std::cout << ct2 << std::endl;
    // Fun(B::i);
    return 0;
}
/*
关于名字查找的一些问题(针对函数调用的情况):
首先，如果通常的无限定查找所生成的集合含有下列任何内容，那么不考虑实参依赖查找(ADL):
1) 类成员的声明
2) 块作用域的（非 using 声明的）函数声明
3) 任何非函数或函数模板的声明（例如函数对象或另一变量，其名字与正在查找的函数名冲突）

也就是说:当经过普通的名字查找后（没有包括ADL），如果候选函数中有类成员、块作用域中的函数声明（不包括using声明引入的）、其他同名的函数对象或变量名，则不启动ADL查找了。
否则对于函数调用的每个参数,都会进行检查
1) 对于基本类型,如int\float\char等,不参与检查!!!!!!
2) 如果是类对象\类模板类型,对于类内部和类定义的命名空间都会检查
3) 如果是枚举类型,对应的类或者命名空间也会检查
4) 指向某个类型的指针,那么对应类型也会被检查(当然如果目标类型是基本类型,实际上还是无效)
5) 函数对象/成员函数类型,对应的类和命名空间也会检查

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


/*
关于std:
std::swap是一个模板函数,我们可以在namespace std中进行特例化,但是不可以进行重载或者是添加新的class/function
*/

/*
std::swap 默认的实现逻辑是:
namespace std{
    template<typename T>
    void swap(T& a, T& b){
        T temp(a);
        a = b;
        b = temp;
    }
}
可见实现依赖的是T的拷贝构造函数和拷贝赋值函数
但是如果我们的模型是pimpl(即类内的成员变量是一个指针,通过这个指针指向了另外的大对象),那么我们在swap时只需要交换两个指针即可,因此std::swap版本并不适用

方式1:特例化std::swap一个函数模板(大部分stl的容器使用的就是这种方式)
namespace std{
    template<>
    void swap<Wiget>(Wiget& a, Wiget& b){ 
        swap(a.ptr, b.ptr) // 使用通用模板对指针进行交换
    }
}
使用上面的方式的前提是:指针变量不是private的

指针变量往往是private的,因此我们还需要在函数内部定义一个成员函数swap,用来操作操作指针
namespace std{
    template<>
    void swap<Wiget>(Wiget& a, Wiget& b){ 
        a.swap(b); // 使用对象的内置swap方法
    }
}

class Wiget{
private:
    WigetImpl *impl;
public:
    void swap(WigetImple &rhs){
        using std::swap;
        swap(impl, rhs.impl); // 使用std::swap的通用版本,这里使用using std::swap而不是直接指定调用std::swap是为了更好的兼容性,例如impl内置有swap,那么如此就会优先调用impl的内置版本
    }
};

以上方法只能适用于非模板的类!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
比方说我们的类定义如下:
template<typename T>
class Wiget{...};
template<typename T>
class WigetImpl{...};
那么关于std::swap的特例化需要是:
namespace std{
    template<typename T>
    void swap<Wiget<T>>(Wiget<T>& a, Wiget<T>& b){ // 这属于对模板函数的偏特化,这种写法是非法的
        a.swap(b);
    }
}

改成:
namespace std{
    template<typename T>
    void swap(Wiget<T>& a, Wiget<T>& b){ // 这属于对模板函数的重载,但是std不允许在该命名空间内进行重载
        a.swap(b);
    }
}

所以为了通用性,我们需要把它定义在std命名空间之外, 一般放在和Wiget,WigetImpl同层的命名空间
template<typename T>
void swap(Wiget<T>& a, Wiget<T>& b){ //现在当调用时,会优先查找到当前命名空间的swap函数(这个命名空间要定义有该swap函数),找到之后即可调用到impl对象内的swap函数 
    a.swap(b);
}

总结:
1. 定义swap需要在一个 member_function 用以操作private的指针成员函数,且该函数是noexcept的
2. 对于单纯的class可以有两种方式:
    1) 特例化std::swap, 这种方式书里面介绍是为了兼容某些程序员std::swap的调用方法,但是我觉得没必要.(这样定义,可以使用std::swap 和 swap 两种调用方式)
    2) 定义non_member_function 的 swap
3. 对于class template, 只能定义 non_member_function 的 swap, 因为需要进行重载,而std并不允许在它的命名空间内重载

*/
