// /*
// 探究sizeof的细则

// 注意:枚举,静态,函数都不会占用类内存

// 内存对齐:
// 1. 对齐方向是对齐类内最大的那个数据类型
// 比如：
// class A
// {
//     public:
//         int a;
//         short b;
//         char c;
//         int d; // 最大的对齐是int类型，因此是 4 + 2 + 1 + (1) + 4 = 12; (1)是内存对齐
// };

// class A
// {
//     public:
//         int a;
//         short b;
//         char c;
//         int d; 
//         double e; // 最大的对齐是double类型，因此是 4 + 2 + 1 + (1) + 4 + (4) + 8 = 24;
// };

// 2. 单个属性时，是不会进行内存对齐的


// 1. 空类的大小为1字节
// 2. 一个类中，虚函数本身(只保存一个指向虚函数表的指针)、成员函数（包括静态与非静态）（存放在代码区，调用方式a.fun => fun(a.this)）和静态数据成员（分配在BSS区域）都是不占用类对象的存储空间。
// 3. 对于包含虚函数的类，不管有多少个虚函数，只有一个虚指针,vptr的大小。
// 4. 普通继承，派生类继承了所有基类的函数与成员，要按照字节对齐来计算大小
// 5. 虚函数继承，不管是单继承还是多继承，都是继承了基类的vptr。(32位操作系统4字节，64位操作系统 8字节)！
// 6. 虚继承,继承基类的vptr。
// */

// // #include<iostream>
// // using namespace std;


// // class A{};
// // int main(){
// //     cout << sizeof(A) << endl; // 空类的大小是1字节，C++要求每个实例在内存中都有独一无二的地址，空类也会被实例化，因此会分配地址，即一个字节的大小
// //     return 0;
// // }

// // #include<iostream>
// // using namespace std;
// // class A
// // {
// //     public:
// //         char b;// 占用类的内存，需要内存对齐，char 1字节，64位系统是8字节对齐，32位是4字节对齐（即 1+7 = 8,总共多分配了7个字节）
// //         virtual void fun() {}; // 只保存一个指向虚函数表的指针
// //         void fun2() {}; // 存放在代码区，不占用类的内存
// //         static int c; // 静态数据成员，分配在BSS段
// //         static int d;
// //         static int f;
// // };

// // int main()
// // {
// //     /**
// //      * @brief 16  字节对齐、静态变量不影响类的大小、vptr指针=8
// //      */
// //     cout<<sizeof(A)<<endl; // 输出16
// //     return 0;
// // }

// // #include<iostream>
// // using namespace std;
// // class A{
// //     virtual void fun();
// //     virtual void fun1();
// //     virtual void fun2();
// //     virtual void fun3();
// // };
// // int main()
// // {
// //     cout<<sizeof(A)<<endl; // 8， 不管有多少个虚函数，都是定义在虚函数表，而类只保存了一个指向该表的虚函数指针
// //     return 0;
// // }

// // #include<iostream>

// // using namespace std;

// // class A
// // {
// //     public:
// //         int b;
// //         char a;
// // };

// // /**
// //  * @brief 此时B按照顺序：
// //  * char a
// //  * int b
// //  * short a
// //  * long b
// //  * 根据字节对齐4+4=8+8+8=24
// //  */
// // class B:A
// // {
// //     public:
// //         short a; // 和C的情况一样，只有当A中成员类型都一样时，才进行拼接。
// //         long b; // 使用gdb可知内存分布如下：$1 = {<A> = {b = 1431652896, a = 85 'U'}, a = 18400, b = 140737488346480}
// // };
// // class C
// // {
// //     A a; // 实测发现，当a中的类型不一致时，不会与C类中的成员进行拼接。比如A{int b; char a;}，B{char c; double d;}， 结果是24; 当A{char b; char a;}，输出为：16
// //     char c; // gdb内存分布： $1 = {a = {b = 0, a = 0 '\000'}, c = 0 '\000'}
// // };
// // class A1 // gdb: $1 = {_vptr.A1 = 0x555555755d80 <vtable for A1+16>}
// // {
// //     virtual void fun(){}
// //     void fun2(){}
// // };
// // class C1:public A1
// // { // gdb 内存分布： $2 = {<A1> = {_vptr.A1 = 0x555555755d68 <vtable for C1+16>}, <No data fields>}
// //     void fun() override{}
// //     virtual void fun3() {}
// //     int j;
// // };

// // int main()
// // {
// //     B b;
// //     C c;
// //     C1 c1;
// //     A1 a1;
// //     cout<<sizeof(A)<<endl; // 8
// //     cout<<sizeof(b)<<endl; // 24
// //     cout<<sizeof(c)<<endl; // 12
// //     /**
// //      * @brief 对于虚单函数继承，派生类也继承了基类的vptr，所以是8字节
// //      */
// //     cout<<sizeof(c1)<<endl; // 8 
// //     return 0;
// // }

// // 虚函数指针（vptr）一般都放在对象内存布局的第一个位置上，这是为了保证在多层继承或多重继承的情况下能以最高效率取到虚函数表。


// #include<iostream>
// using namespace std;
// class A
// {
//     virtual void fun() {}
// };
// class B
// {
//     virtual void fun2() {}
// };
// class C : virtual public  A, virtual public B
// {
//     public:
//         virtual void fun3() {}
// };

// int main()
// {
//     A a;
//     B b;
//     C c;
//     cout<<sizeof(a)<<" "<<sizeof(b)<<" "<<sizeof(c) << endl; // 多继承的情况下，子类会继承父类的虚函数表，即有多个虚函数表

//     return 0;
// }
#include <iostream>
using namespace std;

// class A
// {
//     // int a;
//     virtual void fun1(){}
// };

// class B: virtual public A // 直接使用虚函数继承，如果A没有其他成员变量，那么B会和A共用一个虚函数表，计算出来的sizeof是16
// // 而如果A有其他的成员函数，如int a，则B会创建自己的虚函数表，所以sizeof是32
// {
//     int b;
//     virtual void fun2(){}
// };

// int main()
// {
//     B b;
//     cout << sizeof(b) << endl;
// }




// 菱形继承时的虚函数表现：
class A
{
    int a;
    virtual void fun1(){}
};

class B: virtual public A
{
    virtual void fun2(){}
};

class C: virtual public A
{
    virtual void fun3(){}
};

class D: public B, public C
{
    virtual void fun4(){}
};

int main()
{
    D d;
    cout << sizeof(d) << endl; // 从gdb里面打印的话是8，直接运行结果是16，根据网上资料，应该是8才对，因为A没有其他成员变量，所以
    // B、C不会创建自己的虚函数表，而D是直接继承，也会直接使用A的虚函数表，因此只会有一个虚函数指针，所以sizeof是8
}
/*
当A没有其他成员变量时
(gdb) p d
$1 = {
  <B> = {
    <A> = {
      _vptr.A = 0x61ff80
    }, <No data fields>},
  <C> = {<No data fields>}, <No data fields>}
*/
/*
当a有成员变量时
sizeof结果是32，此时会有三个虚函数表，加一个int类型变量，所以总对象大小为32
三个虚函数表的顺序是：B的虚函数表、C的虚函数表、A的虚函数表；A的虚函数表是被放在最后！！！！
而D自己定义的虚函数，是放在B中，新定义的

*/