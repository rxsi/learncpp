/*
在分布式项目中，各个微服务之间可以相互调用，现在给你一个字符串二维数组call，call[i]=["x","y"]，表示微服务"x"调用微服务"y"，请你判断这个任务数组中是否存在循环调用，如果存在，返回false。不存在返回true
输入：[["A","B"],["B","C"],["C","D"]]
输出：true
输入：[["A","B"],["B","C"],["C","A"],["C","D"]]
输出：false

本质是使用并查集判断是否有环
*/
#include <iostream>
#include <ios>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

unordered_map<string, string> myMap;

string tryFind(string s)
{
    if (myMap[s] == s) return s;
    return tryFind(myMap[s]);
}

bool tryUnion(string s1, string s2)
{
    string f1 = tryFind(s1);
    string f2 = tryFind(s2);
    if (f1 == f2) return false;
    myMap[f1] = f2;
    return true;
}


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<vector<string>> vec{{"A", "B"}, {"B", "C"}, {"C", "A"}, {"C", "D"}};
    for (int i = 0; i < 26; ++i)
    {
        string temp{(char)('A' + i)};
        myMap[temp] = temp; 
    }

    for (int i = 0; i < vec.size(); ++i)
    {
        if (!tryUnion(vec[i][0], vec[i][1]))
        {
            cout << "false" << endl;
            return 0;
        }
    }
    cout << "true" << endl;
    return 0;
}