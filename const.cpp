/*
const 成员变量必须要在类内声明,而通过构造函数初始化
原因:const 数据成员 只在某个对象生存期内是常量，而对于整个类而言却是可变的。因为类可以创建多个对象，不同的对象其 const 数据成员的值可以不同。

如果要创建所有对象都恒定的常量,需要使用枚举,或者使用static const.
注意:static需要在类年声明(带上static),而在类外定义(不加static),如果是static const,则需要加上const
*/
#include<iostream>
using namespace std;

class Test{
public:
    Test():a(0){}
    enum{ // 枚举常量不会占用对象的内存空间,子啊编译时被全部求值
        SIZE = 100, // 但是隐含的数据类型是整数,最大值有限,不能表示浮点数
        COLOR = 200
    };
private:
    const int a; // 需要在构造函数处完成定义赋值
    static int b;
    static const int c;
};

int Test::b = 2;
const int Test::c = 3;

int main(){
    cout << sizeof(Test) << endl; // 输出为4,因为函数,枚举,静态都不占空间,只有int a占4字节的空间
}