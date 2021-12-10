/*
枚举类型

当需要在类内定义一个全部类共用的常量时,有两种方法:
1) 定义 static const, 见 const.cpp文件
2) 使用枚举类型

枚举类型实际上只是一个类型声明,比方说 enum type{SIZE = 1, COLOR = 2}; 实际上是声明了 type 这个类型,
而SIZE, COLOR 只是符号常量(枚举量).代表的是type这个类型的取值访问.
可以这么理解 char 等价于 enum char{0, 1, 2 .... 255}
因此这么定义是不会占用类内存的.

当我们定义对应的类型时,即会产生内存占用,如: enum type{SIZE = 1, COLOR = 2} type1;
当不指定类型时,默认是int类型, 因此 type type1; 占用4字节

*/
#include<iostream>
using namespace std;

class Base{
public:
    enum type{
        SIZE = 1,
        COLOR = 2
    };
    type type1;
};

int main(){
    Base b;
    cout << "before define: " << b.type1 << endl; // 随机值; 当是enum class type{..}时,并不能输出,因此b.type1是 Base::type类型
    b.type1 = Base::type::SIZE; // 使用 Base::SIZE的方式访问,因为enum定义在类Base的作用域内.如果是 enum class type{...}, 则是限定作用域在type里面,因此需要使用 Base::type::SIZE
    cout << "after define: " << b.type1 << endl; // 1
    cout << "sizeof(): " << sizeof(b) << endl; // 不定义枚举变量的话不占用类内存,返回1
}