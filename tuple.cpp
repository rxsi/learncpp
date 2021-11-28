#include<iostream>
#include<tuple>
using namespace std;

int main()
{  
    tuple<string, string> duo("1", "2"); // 值初始化
    tuple<string, string> duoD{"1", "2"}; // 列表初始化
    tuple<string, string> duoDD = {"1", "2"}; // 书上说这个不行,搜索到的讲解也是这么说,但是实际上是可行的,暂时不清楚问题,怀疑1:版本问题;2:编译器主动优化
    
    tuple<string, string> duoDDD = make_tuple("1", "2");
    auto duoDDDD = make_tuple("1", "2"); // 使用make_tuple创建,可以直接使用auto去自动匹配

    auto change = make_tuple(3, 4);
    // get<index>(tuple_name) : 可以返回tuple_name对应index的元素引用
    auto book = get<0>(change);
    get<1>(change) *= 0.8; // 原本的值是整数类型,乘以0.8之后依然会保持为整数,因此结果为3
    cout << get<0>(change) << get<1>(change) << endl;

    cout << tuple_size<decltype(change)>::value << endl; // tuple_size<tuple_type>::value 返回tuple的元素数量
    tuple_element<0, decltype(change)>::type cnt = get<0>(change); // tuple_element<index, tuple_size>::type 返回对应tuple的元素类型

    // tuple的比较必须是元素数量相同且类型相同,且元素应该实现了<号运算符
    // tuple 一般用来返回多个参数, pair只能固定是两个参数
}