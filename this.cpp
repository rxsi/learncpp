/*
this 指针的底层本质
使用断点调试
1. this本质上是一个本身是常量的指向类的指针.
2. 在const成员函数内,则this是一个本身是常量的指向常量类的指针

3. 当调用类的函数时,会将对象地址作为实参传递给this, 不过this这个形参是隐式的,在编译阶段有编译器默认添加到参数列表中
这种方式是__thiscall, 由编译器自动添加.
可以联想:static函数是没有this指针,而虚函数必须要通过vptr指针访问,这个指针是要通过this指针获取到,且当调用时需要传入this指针
因此static不能声明为virtual函数

*/

// #include<iostream>
// #include<cstring>


// using namespace std;
// class Person{
// public:
//     typedef enum {
//         BOY = 0, 
//         GIRL 
//     }SexType;
//     Person(const char *n, int a, SexType s){ // 通过debug调试,发现this指针的类型为 Person *const, 表明this指针本身是指针常量
//         name=new char[strlen(n)+1];
//         strcpy(name,n);
//         age=a;
//         sex=s;
//     }
//     int get_age() const{ // 当在这里的时候,this指针是 const Person *const, 当指向const时,this是一个指向常量的指针常量

//         return this->age; 
//     }
//     Person& add_age(int a){
//         age+=a;
//         return *this; 
//     }
//     ~Person(){
//         delete [] name;
//     }
// private:
//     char * name;
//     int age;
//     SexType sex;
// };


// int main(){
//     Person p("zhangsan",20,Person::BOY); 
//     cout<<p.get_age()<<endl;
//     cout<<p.add_age(10).get_age()<<endl;
//     return 0;
// }

class Test
{
public:
    void fun1() {}
    void fun2() const{}
    virtual void fun3() {}
    virtual void fun4() const {}
};

int main()
{
    Test *t = new Test();
    t->fun1();
    t->fun2();
    t->fun3();
    t->fun4();
}