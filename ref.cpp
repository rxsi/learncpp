#include<iostream>
using namespace std;
/*
从汇编底层理解C++引用
*/

int main()
{
    int x = 4;
    int y = 4;
    int &b = x;
    int &b = y;
    cout << b << endl;
    // int *c = &x;
    // int *d = &b;
    int *const e = &x; 
    return 0;
}
/*
g++ -g -c ref.cpp 
objdump -S ref.o

ref.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <main>:

int main()
{
   0:   55                      push   %rbp  // %rbp是栈帧指针，总是指向当前正在运行的栈帧的起始位置，如果是在32位系统，则是%ebp寄存器
   1:   48 89 e5                mov    %rsp,%rbp // %rsp是栈顶指针，将%rsp寄存器的内容赋值给%rbp，即此时%rbp指向的是栈顶位置
    int x = 1;
   4:   c7 45 f4 01 00 00 00    movl   $0x1,-0xc(%rbp) // $是立即数， (%rbp)表示内存引用， 将0x1存入%rbp内存地址-0xc的位置
    int &b = x;
   b:   48 8d 45 f4             lea    -0xc(%rbp),%rax // 将 -0xc(%rbp) 地址存入 %rax寄存器
   f:   48 89 45 f8             mov    %rax,-0x8(%rbp) // 将 %rax的值存入 -0x8(%rbp)位置
    return 0;
  13:   b8 00 00 00 00          mov    $0x0,%eax // 将立即数0x0存入%eax寄存器
  18:   5d                      pop    %rbp // 弹出%rbp地址
  19:   c3                      retq   
}

# 加入指针之后的反汇编，可以看出指针的处理和引用的处理是一样的
int main()
{
   0:   55                      push   %rbp
   1:   48 89 e5                mov    %rsp,%rbp
    int x = 4;
   4:   c7 45 ec 04 00 00 00    movl   $0x4,-0x14(%rbp)
    int &b = x;
   b:   48 8d 45 ec             lea    -0x14(%rbp),%rax
   f:   48 89 45 f8             mov    %rax,-0x8(%rbp)
    int *c = &x;
  13:   48 8d 45 ec             lea    -0x14(%rbp),%rax
  17:   48 89 45 f0             mov    %rax,-0x10(%rbp)
    return 0;
  1b:   b8 00 00 00 00          mov    $0x0,%eax
}
  20:   5d                      pop    %rbp
  21:   c3                      retq  

疑问1：先压入的x = 1的地址居然是在0xc，但是后面压入的b的地址居然是在0x8.反方向了？
疑问2：实际上压栈操作只有对rbp寄存器地址，而x=1, &b=x只是直接改了后面的内存

结论：虽然是引用，但是实际上保存的还是变量的地址，与指针是一致的
当进行取地址操作时，如果是引用，那么实际上会转换成&(*p)，因此实际上还是会获得x的地址

引用本质上是指针常量(const int *), 因此不允许重绑定：
所以：
int &b = x; // 等价于 const int *p = &x;
&b = y; // 等价于 *p = y，这就报错了！！！！！！！
会编译报错

*/


/*
对于内置类型(如int double), stl迭代器, 函数对象等,直接传递值更为高效

*/