// // #include<iostream>
// // #include<typeinfo>
// // using namespace std;
// // /*
// // 多继承会有菱形继承的问题。
// // Python使用的是C3算法
// // C++使用的是虚继承
// // */

// // class A
// // {
// // public:
// //     A(int i)
// //     {
// //         cout << "initial A with " << i << endl;
// //     }

// //     A()
// //     {
// //         cout << "initial A" << endl;
// //     }

// //     void print_v()
// //     {
// //         cout << "pint_v A" << endl;
// //     }

// //     virtual void vir_print()
// //     {
// //         cout << "vir_print A" << endl;
// //     }
// // };


// // class B: public A
// // {
// // public:
// //     B(int i): A(i)
// //     {
// //         cout << "initial B with " << i << endl;
// //     }
    
// //     B()
// //     {
// //         cout << "initial B" << endl;
// //     }


// //     void print_v()
// //     {
// //         cout << "pint_v B" << endl;
// //     }

// //     void vir_print() override
// //     {
// //         cout << "vir_print B" << endl;
// //     }
// // };

// // class C: public B
// // {
// // public:
// //     C(int i): B(i)
// //     {
// //         cout << "initial C with " << i << endl;
// //     }

// //     C()
// //     {
// //         cout << "initial C" << endl;
// //     }


// //     void print_v()
// //     {
// //         cout << "pint_v C" << endl;
// //     }


// //     void vir_print() override
// //     {
// //         cout << "vir_print C" << endl;
// //     }
// // };


// // // 虚继承是为了解决菱形继承的问题，实际上应该少用，因为本身虚继承会带来其他的一些问题


// // class Base
// // {
// // public:
// //     Base(int i)
// //     {
// //         cout << "initial Base with " << i << endl;
// //     }

// //     Base()
// //     {
// //         cout << "initial Base" << endl;
// //     }
// // };


// // class X: public virtual Base
// // {
// // public:
// //     X(int i): Base(i)
// //     {
// //         cout << "initial X with " << i << endl;
// //     }
    
// //     X()
// //     {
// //         cout << "initial X" << endl;
// //     }
// // };

// // class Y: public virtual Base
// // {
// // public:
// //     Y(int i): Base(i)
// //     {
// //         cout << "initial Y with " << i << endl;
// //     }

// //     Y()
// //     {
// //         cout << "initial Y" << endl;
// //     }
// // };

// // class XY: public X, public Y
// // {
// // public:
// //     XY(int i): X(i), Y(i)
// //     {
// //         cout << "initial XY with " << i << endl;
// //     }

// //     XY()
// //     {
// //         cout << "initial XY" << endl;
// //     }
// // };


// // int main()
// // {
// //     C c;
// //     A &ref_a = c;
// //     cout << typeid(ref_a).name() << endl;
// //     A *point_a = new C();
// //     ref_a.print_v();
// //     ref_a.vir_print(); // 打印C， 具有多态性

// //     point_a->print_v();
// //     point_a->vir_print(); // 打印C，具有多态，父类指针可以指向子类对象


// //     // C c(); // 会默认调用父类的无参构造函数
// //     // A *a = &c;
// //     // C(12); // 正常的初始化构造，会从下往上的调用父类的构造函数，如果不指定调用，则默认调用父类的无参构造函数

// //     // XY();
// //     // XY(3); // 如果没有虚继承，那么Base父类会被构造两次，即有两个Base父类对象被实例化
// //     // //如果是虚继承，那么Base父类将会被构造一次，而且～～～～ 会有XY负责首先调用Base的无参构造函数，再分别调用X、Y的有参构造函数
// //     // // 尽管X、Y的有参构造函数会调用Base的有参构造函数，但是并不会执行！！！！！！！！！！
// // }

// // #include <iostream>
// // using namespace std;

// class A
// {
// public:
//     virtual void func(){}
//     int a = 12;
//     int b = 13;
//     // {
//     //     cout << "A::func" << endl;
//     // }
// };

// class B
// {
// public:
//     virtual void func(){}
//     int a = 22;
//     int b = 14;
//     // {
//     //     cout << "B::func" << endl;
//     // }
// };

// class C: public A, public B
// {
// public:
//     void func() override{}
//     int a = 1;
//     // {
//     //     cout << "C::func" << endl;
//     // }
// };

// // int main()
// // {
// //     A *ac = new C();
// //     ac->func();
// //     B *bc = new C();
// //     bc->func();
// //     C *cc = new C();
// //     cc->A::func();
// //     cc->B::func();
// //     cc->func();
// //     // A *aa = new A();
// //     // aa->func();
// //     // C c;
// //     // c.A::func();
// //     // c.B::func();
// //     // C c;
// //     // c.A::func();
// //     // c.B::func();
// //     // c.func();
// //     return 0;
// // }

// struct A
// {
//     int ax;
//     virtual void f0() {}
//     virtual void f1() {}
//     /* data */
// };

// struct B: public A
// {
//     int bx;
//     void f0() override {} // 而这里由于父类有同名的虚函数，因此编译器会把他放到虚表中
//     virtual void f2() {} // 注意要写成virtual才会在虚表中
//     /* data */
// };


struct A
{
    int ax;
    virtual void f0() {}
};

struct B
{
    int bx;
    virtual void f1() {}
};

struct C : public A, public B
{
    int cx;
    void f0() override {}
    void f1() override {}
};
