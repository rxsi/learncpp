#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

using namespace std;

class Solution {
public:
    static unordered_map<string, int> myMap;

    vector<string> reorderLogFiles(vector<string>& logs) {
        cout << myMap.size() << endl;
        // for (int i = 0; i < logs.size(); ++i) myMap[logs[i]] = i;
        // for (auto iter = myMap.begin(); iter != myMap.end(); ++iter) cout << (*iter).first << " " << (*iter).second << endl;

        // sort(logs.begin(), logs.end(), myCmp()); // 使用stable_sort保证了原始的想对顺序
        return logs;
    }

private:
    // struct myCmp
    // {
    //     bool operator()(const string& s1, const string& s2) // 注意这里使用stable_sort时，不能写成string& s1,内部源码有对参数进行校验，要么传入const string& s1，要么就是string s1
    //     {
    //         int index1 = s1.find_first_of(' ');
    //         int index2 = s2.find_first_of(' ');
    //         if (isdigit(s1[index1+1]) && isdigit(s2[index2+1]))// return false;
    //         {
    //             if (myMap[s1] <= myMap[s2]) return true; // 内部类无法访问外部的非static变量
    //             return false;
    //         }
    //         if (isdigit(s1[index1+1]) && isalpha(s2[index2+1])) return false;
    //         if (isalpha(s1[index1+1]) && isdigit(s2[index2+1])) return true;
    //         if (isalpha(s1[index1+1]) && isalpha(s2[index2+1]))
    //         {
    //             string temp_s1 = s1.substr(index1, s1.size());
    //             string temp_s2 = s2.substr(index2, s2.size());
    //             if (temp_s1 == temp_s2) return s1.substr(0, index1) <= s2.substr(0, index2);
    //             else return temp_s1 <= temp_s2;
    //         }
    //         return true;
    //     }
    // };

    // static unordered_map<string, int> myMap;
};

int main()
{
    Solution s;
    vector<string> v{"ABCD", "DFDS"};
    s.reorderLogFiles(v);
}
// int main()
// {
    
// }
