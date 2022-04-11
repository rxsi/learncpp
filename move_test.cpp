/*
探究move的一些问题
*/
#include <iostream>

class Foo
{
public:
    Foo()
    {
        std::cout << "Foo() " << std::endl;
    }
    Foo(const Foo& other)
    {
        std::cout << "Foo(const Foo& other) " << std::endl;
    }

    Foo& operator=(const Foo& other)
    {
        std::cout << "Foo& operator=(const Foo& other) " << std::endl;
    }
};


int main()
{
    // Foo a, b, c;
    // a = b; 这种方式是先构造a，再调用a的operator=方法

    // Foo b, c;
    // Foo a = b; 这种方式会直接调用a的拷贝构造函数

    Foo a, b, c;
    a = b; // 这里的b是左值，因此匹配了Foo& operator=(const Foo& other) 
    a = std::move(c); // 这里是右值，依然可以匹配Foo& operator=(const Foo& other)

    return 0;
}