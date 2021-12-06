/*
虚函数
C++对象内存布局
*/
// #include<iostream>
// using namespace std;

// class Base{
// public:
//     Base(int i):baseI(i){};
//     virtual void print(){
//         cout << "调用了虚函数Base::print()" << endl;
//     }
//     virtual void setI(){
//         cout << "调用了虚函数Base::setI()" << endl;
//     }

//     virtual ~Base(){}

// private:
//     int baseI;
// };

// int main(){
//     Base b(1000);
//     /*
//     使用 gdb 查看内存分布的流程：
//     1. 安装 gdb
//         $ sudo apt-get update
//         $ sudo apt-get install gdb
//     2. 添加-m32编译支持
//         $ sudo apt-get install build-essential module-assistant  
//         $ sudo apt-get install gcc-multilib g++-multilib  
//     3. 编译文件：
//         g++ <文件名>.cpp -o <文件名> -m64 -g
//     4. 启动gdb
//         gdb <文件名>
//     5. 添加断点
//         b <目标断点位置> ， 示例：b 27
//     6. 运行到断点初
//         run
//     7. 设置打印格式
//         set p pretty on
//     8. 查看目标对象的内存分布
//         p <目标变量>， 例如 b 对象，则 p b
//     9. 输出如下：
//         $1 = {
//             _vptr.Base = 0x56557ebc <vtable for Base+8>, 
//             baseI = 1000
//         }
//     10. p 指令还可以执行其他的运行，如 p sizeof(b),打印b的内存大小
//     10. 退出：
//         q
//     */
//     // 通过gdb打印内存分布，可见b对象的内存起始位置是vptr指针
//     long * vptrAddress = (long*)(&b); // 因此此种方式可以获取到vptr指针
//     cout << "虚函数指针（vprt）的地址是：" << vptrAddress << endl;

//     typedef void(*Fun)(void);
//     Fun vfunc = (Fun)*( (long *)*(long*)(&b)); // 注意，&b是8字节的，因此转成int*会报错，需要使用long*
//     cout << "第一个虚函数的地址是：" << (long *)*(long*)(&b) << endl;
//     cout << "通过地址，调用虚函数Base::print()：";
//     vfunc();

//     Fun vfunc2 = (Fun)*( (long *)*(long*)(&b)+1); // 注意，&b是8字节的，因此转成int*会报错，需要使用long*
//     cout << "第二个虚函数的地址是：" << (long *)*(long*)(&b)+1 << endl;
//     cout << "通过地址，调用虚函数Base::setI()：";
//     vfunc2();
// }

#include<iostream>
#include<typeinfo>
using namespace std;

class Base
{
public:
 
    Base(int i) :baseI(i){};
  
    int getI(){ return baseI; }
 
    static void countI(){};
 
    virtual ~Base(){}

    virtual void print(void){ cout << "Base::print()"; }

    
 
private:
 
    int baseI;
 
    static int baseS;
};

void testBase( Base&p)
{
    cout << "对象的内存起始地址：" << &p << endl;
    cout << "type_info信息:" << endl;
    RTTICompleteObjectLocator str = *((RTTICompleteObjectLocator*)*((int*)*(int*)(&p) - 1));
 
 
    string classname(str.pTypeDescriptor->name);
    classname = classname.substr(4, classname.find("@@") - 4);
    cout <<  "根据type_info信息输出类名:"<< classname << endl;
 
    cout << "虚函数表地址:" << (int *)(&p) << endl;
 
    //验证虚表
    cout << "虚函数表第一个函数的地址：" << (int *)*((int*)(&p)) << endl;
    cout << "析构函数的地址:" << (int* )*(int *)*((int*)(&p)) << endl;
    cout << "虚函数表中，第二个虚函数即print（）的地址：" << ((int*)*(int*)(&p) + 1) << endl;
 
    //通过地址调用虚函数print（）
    typedef void(*Fun)(void);
    Fun IsPrint=(Fun)* ((int*)*(int*)(&p) + 1);
    cout << endl;
    cout<<"调用了虚函数"；
    IsPrint(); //若地址正确，则调用了Base类的虚函数print（）
    cout << endl;
 
    //输入static函数的地址
    p.countI();//先调用函数以产生一个实例
    cout << "static函数countI()的地址：" << p.countI << endl;
 
    //验证nonstatic数据成员
    cout << "推测nonstatic数据成员baseI的地址：" << (int *)(&p) + 1 << endl;
    cout << "根据推测出的地址，输出该地址的值：" << *((int *)(&p) + 1) << endl;
    cout << "Base::getI():" << p.getI() << endl;
 
}