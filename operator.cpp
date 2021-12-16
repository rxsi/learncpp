/*
operator的作用:
1. 重载运算符:
所有赋值相关的operator都应该返回一个引用.
这是因为赋值操作应该具有传递性,如:
int x, y, z;
x = y = z = 13;
实际上编译器的行为如下: x = (y = (z = 13))
因此实际赋值操作符应该为: int& operator=(const int&);



2. 隐式转换:
operator type(){};
*/

#include<iostream>
class A{
public:
    A(int i): num(i){}
    operator int(){ return num; } // 转换为int类型

private:
    int num;
};

int main(){
    A a(10);
    std::cout << a << std::endl; // 输出10
}