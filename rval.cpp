// #include <iostream>
// #include <memory>
// #include <vector>
// #include <string>
// using namespace std;

// struct RValue {
// 	RValue():sources("hello!!!"){
//         cout<<"default init"<<endl;
//     }
// 	RValue(RValue&& a) {
// 		sources = std::move(a.sources);
// 		cout<<"&& RValue"<<endl;
// 	}

// 	RValue(const RValue& a) {
// 		sources = a.sources;
// 		cout<<"& RValue "<<endl;
// 	}

// 	void operator=(const RValue&& a) {
// 		sources = std::move(a.sources);
// 		cout<<"&& =="<<endl;
// 	}

// 	void operator=(const RValue& a) {
// 		sources = a.sources;
// 		cout<<"& =="<<&a<<endl;
// 	}

// 	string sources;;
// };

// RValue get() {
//     RValue a;
// 	return a; // 当return一个对象时会进行一次拷贝，因为是临时量，因此是一个右值，优先匹配移动构造函数。但是现在编译器默认会进行优化
// }

// void put(RValue){}

// template <typename T> void f1(T&);

// int main() {
// //	RValue a = get(); // get() 返回的是一个临时对象，是一个右值，因此优先匹配移动构造函数
// //    cout<<"---------------"<<endl;
// //	put(RValue()); // RValue() 是一个临时量，进行的是移动构造
// //	return 0;

// 	RValue temp = get(); // 移动构造函数
// 	RValue a = temp; // temp 是一个左值，采用拷贝构造函数
//     cout<<"---------------"<<endl;
// 	put(a); // a是一个左值，采用的是拷贝构造
	
// 	int j = 0;
//     // int && rref = j; 这样是错误的，因为j是一个左值，无法绑定到右值引用
//     int && rref = j*2; // 这样是可以的，j×2得到的值是一个右值，注意rref虽然是一个右值引用，但是本身是一个左值
	
// 	f1(j);
// 	// f1(5);
// 	return 0;
	
// }

// 一个普通的左值引用是无法指向右值的,
// 如: int &val = 10; 这是不合法的,因为10只是一个立即数,存放于寄存器中,而不在内存中没有确定的地址
// 但是当我们使用常量引用时,则可以, 即 const int &val = 10; 
// 因为如果是在栈中(比如函数调用内),则会将该立即数存入栈中(汇编可知)
// 如果是在堆中,比如全局作用域,则会存入堆中(汇编可知)
// 二者都会使得有了具体的地址,因此可以产生引用,注意只能是只读操作!!!!!!!!(毕竟是const)

// 右值引用从汇编的角度来看,也是将右值存入内存,但是却可以是读写操作!!!!!!!!!

/*
探究函数(右值引用,左值引用,值传递)的差别
结论:
1. 不允许定义值传递的构造函数,理由是会无限递归.

*/

#include<iostream>
using namespace std;

class Base{
public:
	Base(int size=100): size(size){
		cout << "Base(int)" << endl;
		stack = new int[size];
	}

	~Base(){ // 析构函数必须是无参数的
		cout << "~Base()" << endl;
		delete[] stack;
		stack = nullptr;
	}

	// 1. 值传递构造函数
	// Base(Base base): size(base.size){ // 直接传值会造成无限递归
	// 	cout << "Base(Base)" << endl;
	// }
	/*
	假设可以定义传值的构造函数Base(Base base); 那么 Base b; 
	Base c(b); => Base.c(Base base=b) => Base.c(Base base(b)) 无限递归
	*/

	// 2. 常量引用构造函数
	// 拷贝构造函数的调用时机是本身是一个空对象,准备用传入的已有对象进行赋值,注意和赋值重载的区别
	// 因此不需要先使用 delete[] stack 释放旧内存
	// Base(const Base &base): size(base.size){
	// 	cout << "Base(const Base &base)" << endl;
	// 	stack = new int[base.size];
	// 	for (int i = 0; i < base.size; ++i){
	// 		stack[i] = base.stack[i];
	// 	}
	// }

	// 3. 右值引用构造函数
	// 不需要delete[] stack 释放旧内存
	Base(Base &&base): size(base.size){
		cout << "Base(Base &&base)" << endl;
		stack = base.stack;
		base.stack = nullptr;
	}

	// 1.1 值传递赋值重载
	// Base& operator=(Base base){ // 只有构造函数才可以使用(): size(base.size)
	// // 当使用值传递的赋值重载时,不会发生无限递归,因为本质上形参的复制是常量引用的拷贝构造
	// 	cout << "Base& operator=(Base base)" << endl;
	// 	// if (this == &base){ // 使用常量引用的赋值重载时,要注意判断是否是同一个对象
	// 		// return *this;
	// 	// }
	// 	size = base.size;
	// 	delete[] stack; // 注意要先释放旧的内存空间
	// 	stack = new int[size];
	// 	for (int i = 0; i < size; ++i){
	// 		stack[i] = base.stack[i];
	// 	}
	// 	return *this;
	// }

	// int GetSize(){
	// 	return size;
	// }

	// 2.1 常量引用赋值重载
	// Base& operator=(const Base &base){
	// 	cout << "Base& operator=(const Base &base)" << endl;
	// 	if (this == &base){ // 要注意判断是否是同一个对象
	// 		return *this;
	// 	}
	// 	size = base.size;
	// 	delete[] stack; // 注意要先释放旧的内存空间
	// 	stack = new int[size];
	// 	for (int i = 0; i < size; ++i){
	// 		stack[i] = base.stack[i];
	// 	}
	// 	return *this;
	// }

	// 3.1 右值引用赋值重载
	Base& operator=(Base &&base){
		cout << "Base& operator=(const Base &base)" << endl;
		if (this == &base){ // 要注意判断是否是同一个对象
			return *this;
		}
		size = base.size;
		delete[] stack; // 注意要先释放旧的内存空间
		stack = base.stack; 
		base.stack = nullptr; // 注意要处理原右值的某些属性
		return *this;
	}


	int GetSize(){
		return size;
	}

private:
	int size;
	int *stack; // 整形数组
};

Base GetBase(Base &base){
	Base temp(base.GetSize());
	return temp; // return时会在main栈上产生临时对象,但是现在大多数编译器会自动优化
}

int main(){
	Base s;
	s = GetBase(s);
	return 0;
}

/*
当使用:
	g++ -fno-elide-constructors rval.cpp -o rval
且使用常量引用构造函数和值传递的赋值重载时,输出如下: 2 和 1.1

Base(int)	构造s
Base(int)	构造temp
Base(const Base &base)	在return temp时,将temp拷贝构造给main栈上的临时对象
~Base()		析构temp
Base(const Base &base)	调用值传递的赋值重载时发生的形参拷贝构造
Base& operator=(Base base)	s = GetBase(s) 发生的main栈上的临时对象通过赋值重载运算符赋值给s
~Base()		operator= 的形参析构
~Base()		main栈上的临时对象析构
~Base()		程序运行结束后的s析构

将值传递的赋值重载改成常量引用方式后的输出: 2 和 2.1
Base(int)
Base(int)
Base(const Base &base)
~Base()
Base& operator=(const Base &base) 可见在调用赋值重载时不会发生拷贝,也就没有析构的操作了
~Base()
~Base()

右值引用构造函数和右值引用的赋值重载,3和3.1
Base(int)
Base(int)
Base(Base &&base) 
~Base()
Base& operator=(const Base &base)
~Base()
~Base()
// 虽然调用的步骤不会减少,但是减少了内存拷贝,性能更高
*/