
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

/*
字符串的erase有两种形式
erase(size_type index, size_type count); 这种做法是可以用来删除某个字符
s.find_first_not_of() 第一个没有出现某个char的位置
s.find_first_of() 第一个出现char的位置
s.find_last_not_of() 最后一个没有出现char的位置
s.find_last_of() 最后一个出现cahr的位置
以上返回的是size_type，因此可以结合erase用以删除某个位置的元素

erase(size_type pos); 删除从pos之后的所有元素，默认是0，即删除所有元素
erase(iterator first, iterator last); 

std::remove 是 stl 的函数，用以“移除”指定元素


*/

int main()
{
    vector<int> vec{1, 3, 3, 4, 3, 5};
    auto iter = remove(vec.begin(), vec.end(), 3); // 返回经过“移除”之后，最后位置的迭代器，实际上的删除还需要借助erase方法
    for (auto first = vec.begin(); first != vec.end(); ++first)
    {
        cout << *first << " ";
    }
    cout << endl;
    vec.erase(iter, vec.end());
    for (auto first = vec.begin(); first != vec.end(); ++first)
    {
        cout << *first << " "; // 输出结果是: 1 4 5 4 3 5，remove方法并没有删除元素，只是把后面的非目标元素移到前面，因此容器的size、capacity等参数是不变的
    }
    cout << endl;


    string s = " nice to see you. ";
    // 去除首个空格
    // s.erase(0, s.find_first_not_of(' ')); 用的是erase(size_type index, size_type count)方法，因此不能使用begin
    
    // 去除最后的空格
    // s.erase(s.find_last_not_of(' ')); 用的是erase(size_type pos)

    // 删除所有空格
    // erase-remove 惯用法，用来删除容器中的某些特定元素
    s.erase(remove(s.begin(), s.end(), ' '), s.end()); // 先通过remove将目标元素移到容器末尾，然后再调用erase从iter到end删除
    
    string d;
    d = string(4, 'A') + string(4, 'B');
    cout << d << endl;

}