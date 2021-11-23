#include<iostream>
#include<assert.h>
using namespace std;
 
class A{
public:
  int a;
  A(){
    a1 = 1;
    a2 = 2;
    a3 = 3;
    a = 4;
  }
  void fun(){
    cout << a << endl;    //正确
    cout << a1 << endl;   //正确
    cout << a2 << endl;   //正确
    cout << a3 << endl;   //正确
  }
public:
  int a1;
protected:
  int a2;
private:
  int a3;
};

// public 继承，父类所有的属性的访问修饰不变
// protected 继承，父类的所有属性在该子类中最高变为protected级别
// private 继承，父类的所有属性在该子类中最高变为private级别
// 注意，不同修饰符只是影响了父类属性在该子类的状态，子类依然可以按照原有权限访问父类的属性，只是子类的子类或外部访问会按照改变后的权限访问。
// 友元关系不能继承
class B : private A{
public:
  int a;
  using A::a1; // 将a1的可访问性修改为public
  B(int i){
    A();
    a = i;
  }
  void fun(){
    cout << a << endl;       //正确，public成员
    cout << a1 << endl;       //正确，基类的public成员，在派生类中仍是public成员。
    cout << a2 << endl;       //正确，基类的protected成员，在派生类中仍是protected可以被派生类访问。
//    cout << a3 << endl;       //错误，基类的private成员不能被派生类访问。
  }
};
int main(){
  B b(10);
  cout << b.a << endl; // 输出是10,优先从子类查找属性
  cout << b.a1 << endl;   //private 修饰，则父类的属性在子类中变成了private，无法在类外直接访问，可以使用using改变可访问性
 // cout << b.a2 << endl;   //错误，类外不能访问protected成员,所谓的protected是子类内部才可以访问，外部无法直接调用
  //cout << b.a3 << endl;   //错误，类外不能访问private成员
  return 0;
}