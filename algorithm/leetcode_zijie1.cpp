/*
给你个数字n，一个数组array，从array里面找数字，拼成小于n的最大数
输入:
23123
[2,4,9]
输出：22999

999
[2,4,9]
输出：994
*/


#include <ios>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    // int num, n;
    // cin >> num >> n;
    int num = 999, n = 3;

    // vector<int> vec(n);
    // for (int i = 0; i < n; ++i) cin >> vec[i];
    vector<int> vec{2, 4, 9};
    sort(vec.begin(), vec.end());
    string sNum = to_string(num);
    vector<int> res;
    for (int i = 0; i < sNum.size(); ++i)
    {
        auto iter = find(vec.begin(), vec.end(), sNum[i] - '0');
        if (iter == vec.end()) // 目标值不在列表内，在这里以前的数都是可以匹配上的
        {
            // 尝试找到第一个小于目标值的数
            int minIndex = -1;
            for (int k = 0; k < vec.size(); ++k)
            {
                if (vec[k] < sNum[i] - '0')
                {
                    minIndex = k;
                    break;
                }
            }
            cout << minIndex << " " << i << endl;
            int endPos = sNum.size()-1;
            if (minIndex == -1) // 没有找到比当前值小的数，那么需要减少位数，剩余数都是
            {
                // 从后往前回溯,尝试弹出下标不为0的第一个值
                while (!res.empty())
                {
                    if (res.back() <= 0) res.pop_back();
                    else
                    {
                        res[i]--;
                        break;
                    }
                }
                // 如果res是空的，那么需要减少位数，然后全部填充最大值
                if (!res.empty()) endPos = sNum.size() - res.size(); // 如果不为空，那么说明在前面的while循环中递减了某个值，因此后面都填充最大值即可
            }
            else
            {
                res.push_back(minIndex);
                endPos = sNum.size() - res.size();
            }
            for (int j = 0; j < endPos; ++j) res.push_back(vec.size()-1);
            break; // 退出外层循环
        }
        else // 成功匹配上
        {
            res.push_back(iter - vec.begin());
        }
    }
    // 目标如果是找到小于等于的值，那么这种做法就可以计算出来了
    /*
    string s = "";
    for (auto& index: res) s += to_string(vec[index]);
    */
    // 如果只是要小于的值，那么要计算结果值是否可以原始值相等，如果相等，那么需要从后往前把某个值减小，如果最后整个列表为空，那么就需要减少一位
    int same = 1; // 0表示不一样，1表示一样
    for (int i = 0; i < res.size(); ++i)
    {
        if (sNum[i] - '0' != vec[res[i]])
        {
            same = 0;
            break;
        } 
    }
    if (same)
    {
        while (!res.empty())
        {
            if (res.back() <= 0) res.pop_back();
            else
            {
                res.back()--;
                break;
            }
        }
        int endPos = sNum.size()-1;
        if (!res.empty()) endPos = sNum.size() - res.size();
        for (int j = 0; j < endPos; ++j) res.push_back(vec.size()-1);
    }
    string s = "";
    for (auto& index: res) s += to_string(vec[index]);
    cout << s << endl;
    return 0;
}
