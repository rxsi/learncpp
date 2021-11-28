#include<exception>
/*
当要构造一个捕捉构造函数异常的函数时, 见 template.cpp 示例

使用noexcept可以告知编译器不用考虑异常情况,从而使编译器优化该部分代码的调用,提升效率

*/

void recoup(int) noexcept; // 我们告诉编译器这个函数不会抛出异常
void alloc(int); // 可能会抛出异常  

void f() noexcept
{
    throw std::exception(); // 这种结构属于违反了异常声明,一旦抛出异常,程序会调用terminate结束整个流程,以保证不在运行时抛出异常的承诺
}

void g() noexcept(noexcept(f())); // noexcept(f())会根据f()是否声明了noexcept,而返回true或者false, 
//而noexcept(true/false) 则是声明了本函数是否会抛出异常,因此可以是g与f具有相同的异常类型

void (*pf1)(int) noexcept = recoup; // 一个noexcept指针必须指向一个noexcept的目标
void (*pf2)(int) = recoup; // 一个普通指针可以指向noexcept也可以指向非noexcept

// 父类的虚函数声明了noexcept,则子类也必须声明noexcept
// 父类虚函数没有声明,子类可以声明
// 因为虚函数属于重写,而普通函数属于隐藏,因此普通函数没有这个限制

class Base
{
public:
    virtual void f1() noexcept;
    void f2() noexcept;
};

class Child: public Base
{
public:
    void f1() noexcept override;
    void f2(); // f2可以不声明为noexcept,因为是隐藏
};
