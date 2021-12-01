#include<typeinfo>
#include<iostream>
using namespace std;
/*
判断两个类是否相等
*/

class Base
{
public:
    friend bool operator==(const Base&, const Base&);
protected:
    virtual bool equal(const Base&) const; // 父类引用、指针指向子类对象时，具有多态性
};

class Derived: public Base 
{
protected:
    bool equal(const Base&) const override;
};

bool operator==(const Base &lhs, const Base &rhs) // 定义成友元方法，是为了能够让调用==的时候搜索到类的同层作用域而找得到该函数
{
    cout << "operator==" << endl;
    return typeid(lhs) == typeid(rhs) && lhs.equal(rhs);
}

bool Derived::equal(const Base &rhs) const
{
    auto r = dynamic_cast<const Derived&>(rhs);
    cout << "Derived::equal" << endl;
    return true;
}

bool Base::equal(const Base &rhs) const
{
    cout << "Base::equal" << endl;
    return true;
}

int main()
{
    Derived d1;
    Derived d2;
    Base b1;
    Base b2;
    d1 == d2;
    b1 == b2;
}