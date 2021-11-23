#include <iostream>
#include <func.h>

extern template void Swap(double &a, double &b);

int main()
{
    int n1 = 20, n2 = 10;
    std::cout << "before: n1= " << n1 << " n2= " << n2 << std::endl;

    Swap(n1, n2);
    std::cout << "after: n1= " << n1 << " n2= " << n2 << std::endl;

    double f1 = 23.3, f2 = 33.3;
    std::cout << "before: f1= " << f1 << " f2= " << f2 << std::endl;
    Swap(f1, f2); // 假如没有显式实例化，那么这里在编译的时候会报错，因为只能在func.h头文件中找到声明。而 Swap(int&, int&)则不会报错，原因是func.cpp中的bubble_sort使用了int类型的，实际上进行了实例化d
    std::cout << "after: f1= " << f1 << " f2= " << f2 << std::endl;

    return 0;
}