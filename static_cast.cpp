/*
static_cast 用来进行强制类型转换
*/
// 应用场景1:
#include<iostream>
#include<typeinfo>
using namespace std;
class Base{
public:
    const char& operator[](std::size_t position) const{
        /*
        很多的重复代码
        */
        return text[position];
    }

    char& operator[](std::size_t position){ // 为避免无意义的重写代码,先将非const转为const,然后调用const operator[], 再将结果转换为非const
        return const_cast<char&>(
            static_cast<const Base&>(*this)[position]
        );
    }
private:
    char *text;
};

int main(){
    int a = 1;
    double b = static_cast<double>(a); // 从这句简单的例子可以看出,static_cast是将原有的int对象转换为double对象.
    int c = static_cast<int>(b); // 可以逆向
    cout << "a = " << typeid(a).name() << " b = " << typeid(b).name() << endl;
    // 因此引出了一个容易犯下的错误:
    /*
    class Base{
    public:
        virtual void fun(){ 修改了a的值}
    private:
        int a;
    };

    class Child: public Base{
    public:
        void fun() override(){
            static_cast<Base>(*this).fun(); // 原本的目的是将child对象的a,b都修改.但是实际上a只是在用child中的Base部分创建出来的副本Base对象中被修改,原child对象的a值未被修改.
            // 因此此种方法是错误的.
            修改了b的值
        }
    private:
        int b;
    }

    // 正确做法:
    class Child: public Base{
    public:
        void fun() override(){
            Base::fun(); //在child实例内部调用的其他函数,都会有个隐式的this指针,指向当前实例.
            修改了b的值
        }
    private:
        int b;
    }
    
    */
}
/*
dynamic_cast 是用于在继承链中进行的转换操作
性能低下,应该少使用. 它使用的strcmp比较类名
应该通过虚函数的多态性来实现调用不同的函数版本
*/