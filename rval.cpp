#include <iostream>
#include <memory>
#include <vector>
#include <string>
using namespace std;

struct RValue {
	RValue():sources("hello!!!"){
        cout<<"default init"<<endl;
    }
	RValue(RValue&& a) {
		sources = std::move(a.sources);
		cout<<"&& RValue"<<endl;
	}

	RValue(const RValue& a) {
		sources = a.sources;
		cout<<"& RValue "<<endl;
	}

	void operator=(const RValue&& a) {
		sources = std::move(a.sources);
		cout<<"&& =="<<endl;
	}

	void operator=(const RValue& a) {
		sources = a.sources;
		cout<<"& =="<<&a<<endl;
	}

	string sources;;
};

RValue get() {
    RValue a;
	return a; // 当return一个对象时会进行一次拷贝，因为是临时量，因此是一个右值，优先匹配移动构造函数。但是现在编译器默认会进行优化
}

void put(RValue){}

template <typename T> void f1(T&);

int main() {
//	RValue a = get(); // get() 返回的是一个临时对象，是一个右值，因此优先匹配移动构造函数
//    cout<<"---------------"<<endl;
//	put(RValue()); // RValue() 是一个临时量，进行的是移动构造
//	return 0;

	RValue temp = get(); // 移动构造函数
	RValue a = temp; // temp 是一个左值，采用拷贝构造函数
    cout<<"---------------"<<endl;
	put(a); // a是一个左值，采用的是拷贝构造
	
	int j = 0;
    // int && rref = j; 这样是错误的，因为j是一个左值，无法绑定到右值引用
    int && rref = j*2; // 这样是可以的，j×2得到的值是一个右值，注意rref虽然是一个右值引用，但是本身是一个左值
	
	f1(j);
	// f1(5);
	return 0;
	
}

// 一个普通的左值引用是无法指向右值的,
// 如: int &val = 10; 这是不合法的,因为10只是一个立即数,存放于寄存器中,而不在内存中没有确定的地址
// 但是当我们使用常量引用时,则可以, 即 const int &val = 10; 
// 因为如果是在栈中(比如函数调用内),则会将该立即数存入栈中(汇编可知)
// 如果是在堆中,比如全局作用域,则会存入堆中(汇编可知)
// 二者都会使得有了具体的地址,因此可以产生引用
