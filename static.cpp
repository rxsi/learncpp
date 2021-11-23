/*
静态成员
不与具体的对象关联，只与类相关，存放的位置是在全局变量区
而且不占类的空间，比如用sizeof(A)不会计算static变量的容量
static可以限定使用的作用域
1）在函数内声明，只会被分配一次，作用域是该函数内
2）在模块中声明{},则作用域是在模块内
3）在类内static成员变量，作用域是类
4）在类内static成员函数，作用域是类
由此可见，不能在以上其他地方使用static，因为会使得整个static对整个文件可见，而当某个文件导入了该文件后也就可以访问了，破坏了作用域性质。


1.静态成员函数
a. 不能声明为const函数，因为编译器会为const添加一个隐式的const this指针，类似于python的self，而static是没有this指针的
b. 可以在类内定义，也可以在类内声明，而在类外定义，但是在类外定义时不能加static，只能在声明的语句加static
原因：当允许在百纳量

2. 静态成员变量
a. non-const变量只能是在类内声明，而在类外定义。原因是如果允许在类内定义，那么当某个文件导入该文件（假设是头文件）那么就会存在多个A::i，这破坏了ODR规则
b. const变量可以直接在类内定义

*/


class A
{
public:
    static void test();
    static const void test1(){};

private:
    static int i; 
    static const int period = 30; // 可以在类内直接定义
    static constexpr int grade = 40; // 可以在类内直接定义，使用constexpr编译器会进行校验。
};

void A::test(){} // 不能加static关键字

int A::i = 10;

int main()
{

}
