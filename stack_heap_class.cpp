/*
定义只能在栈/堆上分配的对象
探究虚函数/构造函数在不同访问控制符下的效果

C++ 提供了两种创建对象的方式:
1. 静态创建,即 Base b; 由编译器在栈上移动栈帧指针空出对应的内存,然后调用对象的构造函数,退出栈之后则调用析构函数.因此要求构造函数和析构函数都是可外部访问的
*/

class Base{
public:
    Base() {}
    virtual ~Base() {}
};

class Child: protected Base{
public:
    Child() {}
    virtual ~Child() {}
};


int main(){
    // 1. 构造函数/析构函数都是公有的,则既可以在堆上分配,也可以在栈上分配
    Base b;
    Child c;
    // Base *ptr = new Child();
    // 2. 尝试修改派生类对父类的继承方式为 protected, 会发现无法使用
}