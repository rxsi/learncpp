#include<iostream>
#include<functional>
using namespace std;
/*
函数指针

成员函数指针
*/

/*
函数指针
普通的函数指针是可调用对象
可调用对象分为：
1） 函数指针（成员函数指针不是）
2） 函数对象
3） bind对象
4） lambda表达式
5） function对象
6） 重载了函数调用运算符的类，相当于Python的__call__方法

*/

//1. 普通函数指针和函数对象
void print(const string& s)
{
    cout << "normal function print: " << s << endl;
}

// 函数指针是可调用对象，因此可以直接作为形参使用。
// 注意，是不能定义函数类型的形参的，如果定义了，编译器会自动转换成指向函数的指针
// 如： void print(void pf(const string&)) 会自动转换为 void print(void (*pf)(const string&))

// 返回函数指针
// 注意，不同于形参，编译器不会自动将返回函数类型转换为函数指针
// 方式1）：使用别名
using F = void(*)(const string&); 
F f1()
{
    return print;
}

// 方式2）：使用尾置返回类型
auto f2() -> void(*)(const string&) // 当然这里用 auto f2() -> F 亦可，因为F是别名
{
    return print;
}

// 方式3）：使用decltype，注意返回的是函数类型，因此要手动加上*指针号
decltype(print)* f3()
{
    return print;
}

//2. lambda表达式
// 结构： [从当前作用域开始到上层作用域，捕获的目标值](参数列表) -> 返回类型 {函数体}
// 其中参数列表、返回类型（尾置返回） 可以省略
// 当函数体只有一个return语句时，会自动推算出返回类型，但是如果有多个语句，则会返回void，此时我们就可以明确指定返回类型（尾置返回）了，避免返回类型错误
/// 如：auto f = [](int i) {return i < 0 ? -i : i;}; // 只有一个return，自动推算返回类型
// auto f1 = [](int i) -> int {if (i < 0) return -i; else return i;}; // 这样使用尾置返回说明返回类型

// 1）值捕获， 例：[v1, v2]
// 2）引用捕获， 例：[&v1, &v2]
// 3）隐式捕获，例：[&]lambda体内使用的值都是引用方式使用，[=]拷贝方式引用，[&, v1, v2]除了v1,v2是拷贝方式，其他都是引用方式，[=, v1, v2]除了v1,v2是引用，其他都是拷贝方式


//3. bind
// 用于给函数对象绑定默认的参数
// 假设f有五个参数 auto g = bind(f, a, b, _2, c, _1); 则得到的g只有两个参数g(_1, _2)。当调用g(X, Y)时，实际得到f(a, b, Y, c, X);

//4. function
int add(int, int);
function<int(int, int)> f4 = add; // f4(2, 3) 等价于 add(2, 3)

//5. 重载函数运算符的类
class absInt
{
public:
    int operator() (int val) const // operator()
    {
        return val < 0 ? -val : val;
    }
};


// 类成员指针
// 1） 数据成员指针
// 2） 成员函数指针

// 1. 数据成员指针
class Screen
{
public:
    Screen();
    const string name;

    string get() const
    {
        return name;
    }
private:
    const int num;
};

// 数据成员指针本身不绑定对象，因此只有当解引用时才提供对象信息
// 使用 .* 或者 ->* 方式使用数据成员指针

// 2. 成员函数指针
// 和函数指针类似，也是需要指定返回值和形参
// 成员函数指针不是可调用对象
// 比方说： auto fp = &string::empty;
// find_if(svec.begin(), svec.endl(), fp) fp是一个成员函数指针，是无法调用的，因为会变成fp(*it)，这是普通函数指针的调用方式
// 1）使用function生成一个可调用对象 function<bool (const string&)> fcn = &string::empty;
// find_if(svec.begin(), svec.endl(), fcn), 等价于 fcn(*it) => (*it).*p (p是指向string::empty指针)
// 2）使用mem_fn生成一个可调用对象，无需指定函数返回类型和参数类型，生成的对象含有一对重载的函数调用运算符，一个接收string*，一个接收string&
// find_if(svec.begin(), svec.endl(), mem_fn(&string::empty)),意思是可传入对象也可传入指针
// 3）使用bind生成一个可调用对象
// find_if(svec.begin(), svec.endl(), bind(&string::empty, _1), 同样可以接收对象也可传入指针



int main()
{
    void (*pf)(const string&); // 定义函数指针，需要指定返回值和形参
    pf = print; // 或者使用 pf = &print; 如果是成员函数指针，则只能使用&
    pf("123");  // 或者使用(*pf)("123") 均可输出123
    print("123"); // 直接使用函数对象去调用
    f1()("456");
    f2()("567");
    f3()("678");

    absInt absObj;
    absObj(-1);

    const string Screen::*pdata; // 声明pdata是一个指向Screen类的const string成员的指针
    pdata = &Screen::name; // 赋予地址
    auto pdata2 = &Screen::name; // 或者直接使用auto命令

    Screen myScreen, *pScreen = &myScreen;
    auto s = myScreen.*pdata; // 相当于先*pdata获取到数据成员，然后再使用->或者.的方式访问
    auto s1 = pScreen->*pdata;

    string (Screen::*pmf)() const;
    pmf = &Screen::get; // 指向Screen::get指针，注意不可以类似于普通函数指针那样 pmf = Screen:get;
    string s1 = (pScreen->*pmf)(); // 函数指针调用方式
    string s2 = (myScreen.*pmf)();

}
