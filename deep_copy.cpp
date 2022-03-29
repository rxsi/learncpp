/*
探究深拷贝和浅拷贝

在默认情况下,如果我们没有明确定义拷贝构造函数,那么系统帮我们创建的拷贝构造函数是浅拷贝,如果类成员变量有指针,那么会导致复制了两个指针指向同一个地址
当析构时,就会有一个指针出现指针悬挂(指向未定区域)

因此,我们需要自定义拷贝构造函数,对指针变量进行深拷贝.即将对应的堆内存复制多一份,使两个指针指向不同的内存地址
*/

#include <iostream>

class A
{
public:
    A(int num): data(num)
    {
        ptrData = new int[num]; 
    }

    ~A()
    {
        delete[] ptrData; // 如果是浅拷贝,运行将会报错,因为只是简单的copy了指针,即两个指针指向的是同一个块内存地址,因此当一个对象释放之后,将会造成指针悬挂
    }

    A(const A& obj): data(obj.data)
    {
        ptrData = new int[obj.data]; // 自定义拷贝构造函数,对指针进行深拷贝
    }

    int getData() const
    {
        return data;
    }

private:
    int data;
    int* ptrData;
};


int main()
{
    A a(1);
    std::cout << "A = " << a.getData() << std::endl;

    A b = a;
    std::cout << "B = " << b.getData() << std::endl;

    
}

