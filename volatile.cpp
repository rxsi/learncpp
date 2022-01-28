/*
在release模式下编译,然后使用objdump查看汇编
g++ -O2 -S -masm=intel volatile.cpp
cat volatile.s
*/

// 测试1:volatile可以抑制编译器优化,即将某些没有用的值忽略了

int main()
{
    int a = 5;
}

/*

非volatile
        xor     eax, eax // 可见此处直接被优化掉了,因为a值并没有调用
        
volatile

 mov     DWORD PTR [rsp-4], 5 // 未被优化掉,依然是将立即数$0x5压入栈中(rsp是栈顶指针,栈内存是由高往地变化,因此是-0x4,即4字节)
        xor     eax, eax
*/

// 测试2:volatile可以抑制编译器重排,即语句的前后发生了调换

// volatile int a;
// volatile int b;

// int main()
// {
//     a = b + 1;
//     b = 0;
// }

/*
a, b都非 volatile 版本
        mov     eax, DWORD PTR b[rip]
        mov     DWORD PTR b[rip], 0 //先计算了b=0,发生了重排
        add     eax, 1
        mov     DWORD PTR a[rip], eax 

将a, b其中一个设置为volatile
        mov     eax, DWORD PTR b[rip] // 依然会发生重排,即证明单个属性变量声明为volatile是无用的
        mov     DWORD PTR b[rip], 0
        add     eax, 1
        mov     DWORD PTR a[rip], eax

将a,b都设置为volatile
        mov     eax, DWORD PTR b[rip] 
        add     eax, 1 // 可见没有发生重排
        mov     DWORD PTR a[rip], eax
        xor     eax, eax
        mov     DWORD PTR b[rip], 0
*/

// 测试3:volatile使每次读取都从内存读取
// #include <iostream>

// int fun(int c)
// {
//     return c + 10;
// }

// int main()
// {
//     int a = 5;
//     int b = 10;
//     int c = 20;
//     int d;

//     std::cin >> c;

//     a = fun(c);

//     b = a + 1;
//     d = fun(b);

//     std::cout << a << b << c << d << std::endl;
// }