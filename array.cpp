/*
多维数组实操
new, delete实操

c 使用 malloc 和 free 进行内存空间的申请和释放
c++ 使用 new 和 delete, 更加高效,并且会负责调用构造器和析构函数
注意要配对使用,防止内存泄漏
当使用new申请空间失败后会抛出异常,而malloc则是返回null
*/

#include<iostream>
#include<stdio.h>
#include<string.h>
#include<typeinfo>
using namespace std;

int main()
{
    int *p = new int(5); // 申请一个大小为sizeof(int)的内存空间,值为5
    // int *p = new int; // 默认值为0
    cout << *p << endl;
    delete p;

    char *pp = new char[10];
    // strcpy() C的方法,不推荐使用char数组,应该使用string
}