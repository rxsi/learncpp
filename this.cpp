/*
this 指针的底层本质
使用断点调试
this本质上是一个本身是常量的指向类的指针.
在const成员函数内,则this是一个本身是常量的指向常量类的指针
*/

#include<iostream>
#include<cstring>


using namespace std;
class Person{
public:
    typedef enum {
        BOY = 0, 
        GIRL 
    }SexType;
    Person(const char *n, int a, SexType s){ // 通过debug调试,发现this指针的类型为 Person *const, 表明this指针本身是指针常量
        name=new char[strlen(n)+1];
        strcpy(name,n);
        age=a;
        sex=s;
    }
    int get_age() const{ // 当在这里的时候,this指针是 const Person *const, 当指向const时,this是一个指向常量的指针常量

        return this->age; 
    }
    Person& add_age(int a){
        age+=a;
        return *this; 
    }
    ~Person(){
        delete [] name;
    }
private:
    char * name;
    int age;
    SexType sex;
};


int main(){
    Person p("zhangsan",20,Person::BOY); 
    cout<<p.get_age()<<endl;
    cout<<p.add_age(10).get_age()<<endl;
    return 0;
}