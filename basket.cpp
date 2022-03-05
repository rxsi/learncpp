#include <iostream>
#include <memory>
#include <set>
#include <vector>

/*

*/

class Quote
{
public:
    Quote() = default; // 让编译器提供一个合成的默认构造函数
    // book是一个string对象，使用const string & 是为了可以匹配左值和右值,当去掉const之后就只能匹配左值，则 Quote b("书本", 1.0)将会报错
    // 如果参数是 std::string &book 则我们还可以对传入的book引用进行修改，编程规范不允许这样做，会有风险。
    // double sale_price 直接传入的是值，因此实际上会有拷贝的发生。
    Quote(const std::string &book, double sales_price): 
        bookNo(book), price(sales_price), c(222) {} 
    // 函数后面加const表示本函数不会在内部修改对象，比方说，加了const之后，bookNo = "123"将会报错;也隐式的将this指针定义为了const类型，不允许通过this指针修改内部数据,如this->bookNo = "123";
    // 函数前加const表示返回的是const类型的数据，外部接收者不能修改接收的值
    // 但是如果返回的是普通值，即非指针或引用，如 std::string s = b.isbn(); s ="123"; 这样依然是可以的，因为s.isbn()返回的是一个临时变量（右值），赋值完就销毁了。无关是否是const
    // 但是如果返回的是指针或引用，即是一个左值，那么就会限定的效果。
    // 另外如果实例化了一个const的Quote对象，它只能调用const修饰的函数
    std::string isbn() const {
        return bookNo;
    }

    virtual double net_price(std::size_t n) const {return n * price;}
    virtual ~Quote() = default; // 虚析构函数，使用多态的时候，会从子类到父类逐个调用

    virtual Quote* clone() const & // 引用限定符,this对象指向左值对象,即只有当this对象是一个左值时才可以调用这个函数
    {
        std::cout << "const & " << std::endl;
        return new Quote(*this); // 返回指向新对象的指针
    }

    virtual Quote* clone() && // 引用限定符，this对象指向右值对象，即只有当this对象是一个右值时才可以调用这个函数
    {
        std::cout <<"&& " << std::endl;
        return new Quote(std::move(*this));
    }

    const double& GetPrice() // 如果返回的是 int&，返回的price是一个临时变量，因为还要类型转换为int，因此会报错。
    {
        return price;
    }

    const std::string& GetBookNo()
    {
        return bookNo;
    }

    std::string getS()
    {
        return s1;
    }

    int getC()
    {
        return c;
    }

    int getD()
    {
        return d;
    }


private:
    // 当将变量定义为const时，要么定义时就初始化，即const int a = 1; 要么就要在构造函数中显式的进行初始化构造如 Quote(xxx): a(1){}
    // 而且内置类型数据不会默认初始化，如const std::string bookNo;会被默认初始化为空，但是const int a;不会初始化，这是兼容C语言的原因
    std::string bookNo; 

    const std::string s1;
    const int c = 111;
    int d;

protected:
    double price = 0.0;
};

class Basket
{
public:
    // 注意，如果直接存储的是Quote类，那么当我们存入Quote的子类时，将会发生切断问题（子类-），因此使用Quote指针，形成多态
    // 因为参数是指针，因此传入时需要进行手动的转换，如 bsk.add_item(make_shared<Quote>("123",45));比较麻烦，故可设计成隐藏指针版本
    // void add_item(const std::shared_ptr<Quote> &sale)
    // {
    //     items.insert(sale);
    // }
    // 注意，当使用这种方式时，如果是new Quote(sale) 的方式，将会导致Quote子类变成父类，导致发生切断
    // 因此我们需要使不同的类调用不同的new方式，因此是多态的，也就是需要使用到虚函数
    void add_item(const Quote &sale); // 拷贝版本
    void add_item(Quote &&sale); // 移动版本

    double total_receipt(std::ostream&) const; // 声明函数

private:
    // multiset作为一个全局的函数，是不能依赖于对象的（无法提供this指针），因此如果cmpare是非static函数，那么就是一个成员函数，需要隐式的this指针作引导
    // 故而只能定义为static函数
    // 当然如果定义为全局函数亦可
    static bool compare(const std::shared_ptr<Quote> &lhs, const std::shared_ptr<Quote> &rhs)
    {
        return lhs->isbn() < rhs->isbn();
    }

    std::multiset<std::shared_ptr<Quote>, decltype(compare)*> items{compare}; // 注意，初始化时使用的是{}，发现vector也一样，后面再细究～～～～～～～～
};

void Basket::add_item(const Quote &sale)
{
    items.insert(std::shared_ptr<Quote>(sale.clone()));
}

void Basket::add_item(Quote &&sale)
{
    items.insert(std::shared_ptr<Quote>(std::move(sale).clone())); // 注意，这里要使用std::move()，因为右值引用本身是一个左值
}

double print_total(std::ostream&, const Quote&, size_t); // 如果函数的调用在定义它之前，那么需要在调用之前补上声明

double Basket::total_receipt(std::ostream &os) const
{
    double sum = 0.0;
    // 使用cbegin,cend是避免通过iter修改了items的内容，upper_bound()指向所有于*iter关键字相同的元素中的最后一个的下一个位置
    // *iter返回的是Quote智能指针
    for (auto iter = items.cbegin(); iter != items.cend(); iter = items.upper_bound(*iter)) 
    {
        sum += print_total(os, **iter, items.count(*iter)); // **iter返回的是Quote/Quote的子类，因为多态
    }
    os << "Total Sale: " << sum << std::endl;
    return sum;
}

double print_total(std::ostream &os, const Quote &item, size_t n)
{
    double ret = item.net_price(n);
    os << "ISBN: " << item.isbn() << " # sold: " << n << " total due: " << ret << std::endl;
    return ret;
}

int num = 10;
const int GetA()
{
    return num;
}

const int& GetB()
{
    return num;
}

int main()
{
    Quote q("书本", 123);
    Basket bsk;
    bsk.add_item(q);

    bsk.add_item(std::move(q));

    int a = GetA();
    std::cout << "get a = " << a << std::endl;
    a = 20;
    std::cout << "get a = " << a << std::endl;

    int b = GetB(); // 这里虽然GetB返回的是const引用，但是因为这种形式是拷贝复制，因此b得到的只是副本而不是引用，因此 int b 和 const int b都可以
    std::cout << "get b = " << b << std::endl;
    b = 30;
    std::cout << "get b = " << b << std::endl;
    
    const int& b2 = GetB(); // 使用引用去接数据时，则必须使用 const int&
    std::cout << "get c = " << b2 << std::endl;

    std::cout << q.GetPrice() << "  " << q.GetBookNo() << std::endl;

    std::cout << "get = " << q.getC() << q.getS() << q.getD() << std::endl; // 
}
/*
引用说明：
1. 不要返回临时变量的引用，比如说函数内临时定时的变量，因为函数退出时会被销毁，编译会直接报错！！
2. 最好不要返回函数内部new的对象，不合规范
3. 可以返回类内的变量或者全局变量的引用，但是注意，返回的引用类型要一致，否则会因为将要发生类型转换而导致实际
返回的值是临时变量，从而编译报错

*/