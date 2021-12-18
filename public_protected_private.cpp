/*
访问控制符
*/

class Base{
public: // 可以在本类/子类/外部访问,外部访问指的是可以通过实例对象调用
    int a = 0;
protected: // 只能在本类/子类内部访问,因此无法通过实例对象调用
    int b = 1;
private: // 只能在本类内部访问
    int c = 2;
};

class PublicChild: public Base{

};

class PublicChild2: public PublicChild{

};

class ProtectedChild: protected Base{

};

int main(){
    PublicChild pub;
    pub.a; // 只能访问到a
    PublicChild2 pub2;
    pub2.a; // 依然可以访问a

    ProtectedChild pro;
    // pro.a; // 无法调用,因为是protected继承,只能在子类的内部调用
}

