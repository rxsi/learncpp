/*
1. #define 宏定义
不是函数,是在预编译阶段会进行简单的字符替换,省去了函数调用的入栈出栈开销.
但是不会进行类型检查,且某些时候会有问题,比如
# define F(x) X+X
F(i++) 会被展开会 (i++ + i++), 造成BUG
当我们给F(x)传递不能+的类型时,预编译也无法检测出来,同样造成BUG

2. inline 内联函数
本质上是函数,但是只能是简单的函数,不能包含复杂的控制语句:while\swtich等,且不能是递归.
如果内联的函数不满足"简单"的条件,那么编译器会当成一个普通函数调用
是在编译期进行替换,同样节省了函数调用的入栈出栈开销
会进行类型的检查
但是当有多处调用时,会在每个调用处都进行展开,导致函数体变大,内存开销变大
在类中定义的成员函数都是默认的内联函数(除了虚函数,虚函数需要显式的添加inline),而只是声明函数,则需要把类外定义的函数加上inline修饰


 * 编译器对 inline 函数的处理步骤
 * 将 inline 函数体复制到 inline 函数调用点处；
 * 为所用 inline 函数中的局部变量分配内存空间；
 * 将 inline 函数的的输入参数和返回值映射到调用方法的局部变量空间中；
 * 如果 inline 函数有多个返回点，将其转变为 inline 函数代码块末尾的分支（使用 GOTO）。
 
*/

// inline要与函数定义放在一起,函数声明可以不加inline
void Foo(int x, int y);
inline void Foo(int x, int y)
{
    ;
}

// 内联函数应该定义在头文件中,而普通的函数可以生命观在头文件,然后在源文件进行定义


/*
编译的整个过程:
未编译          预编译           编译            汇编               链接
func.h          func.i          func.s          func.o          projectname.exe
func.cpp        main.i          main.s          main.o
main.cpp

1. 预编译:(主要做4件事)
1) 展开#include的头文件
2) 宏替换
3) 去掉注释
4) 条件编译 #ifndef #define #endif

2. 编译:(将代码转换为汇编代码)
1) 编译器为每个源文件维护一个函数地址符表,该表保存当前源文件内包含的各个函数的地址(会重写函数名,以解决函数重载,对于C函数,则是通过extern "C"避免重写函数名)
2) 进行 词法分析 -> 语法分析 -> 语义分析, 函数调用会被编译成call指令,call指令后面跟的是
jmp指令的汇编代码地址，而jmp指令后面跟的才是“被调用的函数编译成汇编代码后的第一条指令”的地址，
但是给call指令后面补充上地址的工作是在链接的时候做的事情。

3. 汇编:
转换成机器语言(二进制)

4. 链接:
将多个.o文件链接到一个可执行的.exe文件;
但是在这个过程中，编译器做的一个重要的事情是将每个文件中call指令后面的地址补充上；
方式是从当前文件的函数地址符表中开始找，如果没有，继续向别的文件的函数地址符表中找，
找到后填补在call指令后面，如果找不到，则链接失败。

相较于汇编产生的二进制文件,链接之后的二进制文件明显更加大了.因为补全了函数调用


*/

/*
虚函数可以是内联函数，内联是可以修饰虚函数的，但是当虚函数表现多态性的时候不能内联。
内联是在编译器建议编译器内联，而虚函数的多态性在运行期，编译器无法知道运行期调用哪个代码，
因此虚函数表现为多态性时（运行期）编译器不会使之内联。
inline virtual 唯一可以内联的时候是：
编译器知道所调用的对象是哪个类（如 Base::who()），
这只有在编译器具有实际对象而不是对象的指针或引用时才会发生。
*/

#include <iostream>  
using namespace std;
class Base
{
public:
    inline virtual void who() // 显式的指定为内联
    {
        cout << "I am Base\n";
    }
    virtual ~Base() {}
};
class Derived : public Base
{
public:
    inline void who()  // 不写inline时隐式内联
    {
        cout << "I am Derived\n";
    }
};

int main()
{
    // 此处的虚函数 who()，是通过类（Base）的具体对象（b）来调用的，编译期间就能确定了，
    // 所以它可以是内联的，但最终是否内联取决于编译器。 
    Base b;
    b.who();

    // 此处的虚函数是通过指针调用的，呈现多态性，需要在运行时期间才能确定，所以不能为内联。  
    Base *ptr = new Derived();
    ptr->who();

    // 因为Base有虚析构函数（virtual ~Base() {}），所以 delete 时，会先调用派生类（Derived）析构函数，再调用基类（Base）析构函数，防止内存泄漏。
    delete ptr;
    ptr = nullptr;

    system("pause");
    return 0;
} 