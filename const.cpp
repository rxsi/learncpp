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
    Test():a(0){} // :a(0)是直接初始化, 而 {a(0)}是先对a进行默认初始化，再进行赋值,二者不一样,初始化效率更高;
    // 赋值版本会调用default构造函数为a赋初值,然后再进行赋值.
    // 编译器会自动调用非内置类型的成员属性的default构造函数,比如string.而不会调用内置类型的,因此如果不初始化a,那么会是一个随机值
    // 当然为了避免这种情况都显式的初始化即可.
    enum{ // 枚举常量不会占用对象的内存空间,在编译时被全部求值
        SIZE = 100, // 但是隐含的数据类型是整数,最大值有限,不能表示浮点数
        COLOR = 200
    };
    void change() const;
private:
    const int a; // 需要在构造函数处完成定义赋值
    mutable int k; // 声明总是可以改变,即使在const函数中
    static int b;
    static const int c; // 注意这种形式属于类内声明而在类外定义
    // 当我们需要在类内使用该c值时,必须要在类内先定义.此时做法是:
    static const int d = 5; // 注意这种做法某些编译器不支持.如果不支持又想要使用该值,那么只能使用enum形式了.
    string s;
};

void Test::change() const{
    //a = 1; // 这样是无法修改的,因为函数声明了是const的.
    k = 2; // 可以修改,因为是multable
}

int Test::b = 2;
const int Test::c = 3;
const int Test::d;

int main(){
    cout << sizeof(Test) << endl; // 输出为4,因为函数,枚举,静态都不占空间,只有int a占4字节的空间
}