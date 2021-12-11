/*
编译器默认为我们构造的函数
*/
#include<iostream>
using namespace std;

class Base{
public:
    Base() = default; // 如果参数是引用类型
    Base(int &m): i(m){}
    const int i;
    // int &j;
};
/*
Base 是一个空的类
当我们使用时,编译器会自动创建4种函数(注意,是当使用到某个函数时才会创建)
1. Base() {} 空的
2. ~Base() {} 正确的析构
3. Base(const Base&){} 对各成员属性进行复制
4. Base& operator=(const Base&){} 对各成员属性进行复制

以上的函数与我们直接使用:
1. Base() = default
2. ~Base() = default
3. Base(const Base&) = default
4. Base& operator=(const Base&) = default
效果一致

=default的目的是当我们想要使用编译器为我们自动生成的函数时,即可使用

当我们定义了自己实现的构造函数之后如Base(int i): a(i){},编译器并不会默认为我们构造默认构造函数,即 Base(){}
此时需要我们显式的定义默认构造函数,以实现一些默认的构造.
这时候要优先选用=default,因为他会实现一些constexpr的特性

如果某个成员变量是引用 或者是 const 类型,那么:
1) 必须使用自定义的初始化函数 Base b(int &j): i(j){}, 且参数必须是引用,且不能放在{}里面进行赋值,因为这实际上是先初始化后再赋值.与引用/const性质冲突
2) 因为引用/const必须要初始化,因此不能有调用默认无参构造函数,尽管你可以定义,但是Base b; 将会报错

*/


int main(){
    // Base b(1, "abc", 10.3); // 创建 Base(){}默认构造函数 和 ~Base(){}析构函数
    // cout << "b.x = " << b.x << " " << "b.s = " << b.s << " " << "b.d = " << b.d << endl;
    // Base b2(b); // 创建Base(const Base& base){} 拷贝构造函数
    // cout << "b2.x = " << b2.x << " " << "b2.s = " << b2.s << " " << "b2.d = " << b2.d << endl;
    // b2 = b; // 创建Base& operator=(cosnt Base& base){} 赋值重载函数
    int a = 1;
    Base b(a);
    cout << " b.i = " << b.i << endl;
    // Base b2;
    // cout << " b2.i = " << b2.i << endl;
}