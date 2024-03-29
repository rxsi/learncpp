// #include <iostream>
// #include <vector>
// #include <memory>
// #include <initializer_list>
// #include <type_traits>

// /*
// 当比如初始化为Blob<int> 时，编译器会实例化出一个与下面等价的类：
// 且成员函数只有在被使用的情况下才会实例化
// template <> class Blob<int>{...}

// 一般把模板函数/类定义在头文件中

// 定义在类模板中的成员函数被隐式的声明为了内联函数～～～～～～

// 在一个类模板的作用域内，可以直接使用类名Blob，而不需要Blob<T>

// 模板函数/模板类都可以有默认值，在C++11之前模板函数不可以使用默认值
// 模板类在多态情况下需要指定参数，分模板子类+模板父类 和 普通子类+模板父类

// */


// template <typename T>
// class BlobPtr{};

// template <typename T>
// class Blob
// {
// public:
//     typedef T value_type;
//     typedef typename std::vector<T>::size_type size_type; // typename声明这个一个T的type，而不是属性，比如 T::SubType * ptr;如果是这样，那么相当于使用的是T的SubType属性，*变成了乘法;
//     // 而 typename T::SubType * ptr，则是表明了Subtype是一个type（类型名），ptr是一个指针
    
//     /*
//     友元相关
//     */
//     friend T; // 声明类型T为模板的友元，即Foo是Blob(Foo)的友元
//     template <typename X> friend class BlobPtr; // 将某个模板的所有实例都声明为友元，注意模板参数要不一样。
//     friend class BlobPtr<T>; // 将相同类型的BlobPtr声明为友元
//     // ==================================================================

//     /*
//     构造函数
//     */
//     // 成员模板
//     // 可以在一个类内（模板类/非模板类均可）定义成员模板，用意是使得该函数具有泛型
//     template <typename It> Blob(It b, It e); // 声明模板函数，注意模板参数要与类模板的参数名字不同，且类外定义时需要同时加上类模板和函数模板前缀
//     Blob();
//     Blob(std::initializer_list<T> il);
//     // ======================================================================================================

//     size_type size() const { return data->size(); }
//     bool empty() const { return data->empty(); }
//     void push_back(const T &t) { data->push_back(t); }
//     void push_back(T &&t) { data->push_back(std::move(t)); }
//     void pop_back();
//     T &back();
//     T &operator[](size_type i); // 下标运算符，可以定义多一个常量的，如 cont T &operator[](size_type i) const;

// private:
//     std::shared_ptr<std::vector<T>> data;
//     void check(size_type i, const std::string &msg) const; // 函数声明

// };

// template <typename T>
// void Blob<T>::check(size_type i, const std::string &msg) const // 外部定义函数，需要加上template 声明
// {
//     if (i >= data->size()) throw std::out_of_range(msg);
// }

// template <typename T>
// T &Blob<T>::back()
// {
//     check(0, "back on empty Blob");
//     return data->back();
// }

// template <typename T>
// T &Blob<T>::operator[](size_type i)
// {
//     check(i, "subscript out of range");
//     return (*data)[i];
// }

// template <typename T>
// void Blob<T>::pop_back()
// {
//     check(0, "pop_back on empty Blob");
//     data->back();
// }

// /*
// 构造函数定义
// */
// template <typename T>
// Blob<T>::Blob(): data(std::make_shared<std::vector<T>>()) {}

// template <typename T>
// Blob<T>::Blob(std::initializer_list<T> il): data(std::make_shared<std::vector<T>>(il)) {} // initialize_list不定参数，使得可以使用 Blob<string> s = {"a", "b", "c"};

// /*
// try...catch 一个构造函数
// */
// template <typename T>
// Blob<T>::Blob(std::initializer_list<T> il) try: data(std::make_shared<std::vector<T>>(il)){}
// catch(const std::bad_alloc &e) {/* do sommeting*/} // catch使用引用,才可以修改异常,比如可以修改后再次throw抛出给上层


// template <typename T>
// template <typename It>
// Blob<T>::Blob(It b, It c): data(std::make_shared<std::vector<T>>(b, c)) {} // 接收不同类型的迭代器，用以初始化vector，比如Blob<int> a1(vi.begin(), bi.end());
// // ============================================================================================


// template <typename T> using vec1 = std::vector<T>; // 定义类模板别名,此方法的好处是无需指定具体的类型 vec<std::string> s = {"a", "b", "c"};，且如果有多个参数还可以指定某个参数的类型如：
// template <typename T> using partNo = std::pair<T, unsigned>; //后一个参数被指定为了unsigned，partNo<std::string> books即可完成初始化,且不可定义第二个参数如：partNo<std::string, int> books2; 是错误的
// // 模板函数的类型转换只能是在两种情况：1）非const -> const 2）函数/数组 -> 指针
// // 但是1）含有普通类型参数，如template <typename T> ostream &print(ostream &os, const T &obj); ostream可以类型转换，如ofstream转
// // 2)显式指定了某个参数后，该参数也可以进行转换，如上面的partNo的第二个参数，或者下面情况
// template <typename T1, typename T2, typename T3> T1 sum(T2 t2, T3 t3)
// {   
//     return static_cast<T1>(t2 + t3);
// }
// typedef std::vector<std::string> vec; // 定义特定类型的别名，必须要指定某种类型，因为模型不是类，所以必须显式指定

// // 动态返回类型，因为我们可以知道传入参数的类型，但是不能确定返回的类型，比如传入的是string迭代器，目的是返回string
// // 可以使用尾置返回类型,获得的是左值引用，因为*beg解引用返回的是一个左值
// template <typename It> auto fcn(It beg, It end) -> decltype(*beg) { return *beg; }  
// // 获取返回类型，而不是引用,注意type是一个类成员，因此属于调用模板参数，需要加typename修饰
// template <typename It> auto fcn2(It beg, It end) -> typename std::remove_reference<decltype(*beg)>::type { return *beg;}

// /*
// 转发
// */
// template<typename F, typename T1, typename T2> void flip1(F f, T1 t1, T2 t2) // 这个版本只能匹配原类型，即左值右值都只能转换为原类型
// {
//     f(t1, t2);
// }
// // 可以使用我们引用折叠，保持参数的原类型
// // 但是还是有局限性，无法向f传递右值，尽管t1可能是一个右值引用，但是右值引用本身是一个左值，因此仍然无法向f传递右值
// // 当然可以直接使用std::move(t1)转换为右值，但是这样不具有通用性
// // 使用std::forward<>即可保持类型的原型
// template<typename F, typename T1, typename T2> void flip2(F f, T1 &&t1, T2 &&t2)
// {
//     // f(t1, t2);
//     f(std::forward<T1>(t1), std::forward<T2>(t2));
// }


// void f(int v1, int &v2)
// {
//     std::cout << v1 << " " << ++v2 << std::endl;
// }

// void g(int &&v1, int &v2)
// {
//     std::cout << v1 << " " << ++v2 << std::endl;
// }


// template <typename T>
// T max(T a, T b)
// {
//     return a > b ? a : b;
// }

// template <typename T>
// T max2(T& a, T& b)
// {
//     return a > b ? a : b;
// }

// int main()
// {
//     partNo<std::string> books;
//     int i = 0;
//     long lng = 1234;
//     auto val3 = sum<long long>(i, lng); // 因为T1的类型不能根据参数推测出，因此需要显式指定
//     // 但是注意，如果改了顺序 template <typename T1, typename T2, typename T3> T3 alternative_sum(T2, T1), 初始化需要逐个显式化指定 auto val3 = alternative_sum<long long, int, long>(i, lng);
//     std::cout << "sum = " << val3 << std::endl;
    
//     /*
//     转发
//     */
//     int j = 0;
//     f(42, j); // j是一个左值，运行后输出42, 1，且j = 1发生了改变
//     std::cout << "j = " << j << std::endl;
//     flip1(f, 42, j); // 运行后输出42, 2，但是j = 1没有改变，实例化为 void flip1(void(*f) (int, int&), int t1, int t2)，如果需要匹配左值引用，需要定义为flip1(F f, T1 t1, T &t2);
//     std::cout << "j = " << j << std::endl;
//     flip2(f, 42, j); // 运行后输出42, 2，且j = 2发生了改变
//     std::cout << "j = " << j << std::endl;
//     flip2(g, 42, j); // 如果不使用std::move/std::forward<>则无法通过编译，因为无法向g传递右值
//     std::cout << "j = " << j << std::endl;
    
//     int a = 10;
//     const int b = 11;
//     ::max(a, b); // 因为是值传递,因此可以类型退化,即const int b 会退化成 int b 

//     //::max2(a, b); // 因为是引用传递,两个参数类型必须一致,因为这里会报错,把a改成 const int a则不会报错

// }

// #include <vector>
// #include <string>

// template <typename T>
// class MyStack
// {
// private:
//     std::vector<T> elems;

// public:
//     MyStack(const T& elem): elems({elem}){}
// };

// MyStack(const char*) -> MyStack<std::string>;

// int main()
// {
//     MyStack<std::string> stringStack{"bottom"};
//     return 0;
// }

// #include <iostream>
// #include <string>
// #include <initializer_list>

// void print()
// {
//     std::cout << "empty" << std::endl;
// }

// template <typename T, typename... Types>
// void print(T firstArg, Types... args)
// {
//     std::cout << firstArg << std::endl;
//     print(args...);
// }

// template <typename F, typename... Types>
// void print(const F& f, Types... args)
// {
//     std::initializer_list<int>{(f(args), 0)...};
// }

// template <typename T>
// void printArg(T t)
// {
//     std::cout << t << std::endl;
// }

// template <typename... Types>
// void expand(Types... args)
// {
//     int arr[] = {(printArg(args), 0)...};
// }


// int main()
// {
//     print([](int i) {std::cout << i << std::endl;}, 1, 2, 3);
//     expand(1, 2, 3, 4);
// }

// 非依赖基类
// template <typename X>
// class Base
// {
// public:
//     int basefield;
//     using T = int;
// };

// template <typename T>
// class D2: public Base<double>
// {
// public:
//     void f()
//     {
//         basefield = 7;
//     }
//     T stragne; // 这里会查询到Base中的T
// };

// void g(D2<int*>& d2, int* p)
// {
//     d2.stragne = p; // 这里在编译期之前就会提示异常
// }

// 依赖基类
// template <typename T>
// class DD: public Base<T>
// {
// public:
//     void f() {std::cout << this->basefield << std::endl;} 
//     // 如果这里使用的是basefield，那么编译时会报错，
//     // 因为没有声明是依赖模板参数，编译器会要求在编译前就可以找到对应的定义，不会进入到第二阶段查找，因为会报错
//     // 要声明this->basefield或者 Base<T>::basefiled（这个会影响多态性）
// };

// template <>
// class Base<bool>
// {
// public:
//     enum {basefield = 42};
// };

// void g(DD<bool>& d)
// {
//     d.f();
// }

// int main()
// {
//     DD<bool> d;
//     g(d);
// }

// template <typename T>
// class B
// {
// public:
//     enum E {e1 = 6, e2 = 26, e3 = 23};
//     virtual void zero(E e = e1);
//     virtual void one(E&);
// };

// template <typename T>
// class D: public B<T>
// {
// public:
//     void f()
//     {
//         std::cout << "D:f()" << std::endl;
//         typename D<T>::E e; // 这里不能使用this->E，因为typename的原因，其他情况下要优先使用this->
//         this->zero(); // 这里如果使用了D<T>::E或者B<T>::E都不能实现多态性
//         this->one(e); // 这里《C++ template》说可以不同加this->，但是实际上是需要的
//     }
// };

// // 如果有多个模板类定义了相同的属性，那么需要使用using指定
// template <typename T>
// class DD3: public Base<T>
// {
// public:
//     using Base<T>::basefield; 
//     void f() {basefield = 0;}
// };

// int main()
// {
//     // D<int> d;
//     // d.f();
//     return 0;
// }

// #include <iostream>
// using namespace std;

// template <typename T>
// T max(T a, T b)
// {
//     return a < b ? b: a;
// }

// int main()
// {
//     int c = ::max<>(1, 2);
//     int d = ::max<int>(2, 3);
//     cout << d << endl;
//     int a = 1;
//     double b = 1.2;
// }
#include <string>
#include <iostream>
#include <vector>
using namespace std;

// 变参
// void print(){} //这里定义空的函数是为了当变参为空时调用该函数，否则会异常报错。

// template <int... Types>
// void print(int firstArg, int... args)
// {
//     std::cout << firstArg << std::endl;
//     print(args...);
// }

// 应用

int main()
{
    // vector<int> init{1, 2, 3};
    // vector<int> init2{2, 3, 4};
    // vector<vector<int>> container;
    // container.emplace_back(init.begin(), init.end());
    // container.emplace_back(init2.begin(), init2.end());
    // // vector<int> c(v.begin(), v.end());
    // for (auto& v: container)
    // {
    //     for (auto& c: v) cout << c << ", ";
    //     cout << endl;
    // }
    // int tempSum = 0;
    // calSum()
    // print(7, 3, 2);
    int a[] = {1, 2, 3};
    int n = sizeof(a) / sizeof(a[0]);
    // vector<int> c(a, a + n); // 数组指针可以作为iterator，因此此处可以成立；
    // vector<int> c(a, *(&a + 1)); // 这样也可以
    vector<int> (std::begin(a), std::end(a)); // 也可以这样，底层的实现就是上面的那种：
    /*
      template<typename _Tp, size_t _Nm>
    inline _GLIBCXX14_CONSTEXPR _Tp*
    begin(_Tp (&__arr)[_Nm])
    { return __arr; }

      template<typename _Tp, size_t _Nm>
    inline _GLIBCXX14_CONSTEXPR _Tp*
    end(_Tp (&__arr)[_Nm])
    { return __arr + _Nm; }
    
    */
    return 0;

}