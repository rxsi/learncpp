/*
基数排序，一般来说如果只是计算正整数，那么分为10个桶即可，
如果要计算负整数，那么就需要分成20个桶。
*/
#include <vector>
#include <iostream>
using namespace std;

int calNumLen(vector<int>& data)
{
    int maxNum = INT_MIN;
    for (auto num: data)
    {
        if (num < 0) num = -num;
        maxNum = max(maxNum, num);
    }
    int count = 0;
    while (maxNum)
    {
        count++;
        maxNum /= 10;
    }
    return count;
}


void radixSort(vector<int>& data)
{
    int maxNumLen = calNumLen(data);
    vector<vector<int>> bucket(20, vector<int>()); // 0-9存负整数，10-19存正整数
    int rate = 1;
    for (int i = 0; i < maxNumLen; ++i)
    {
        for (auto num: data)
        {
            if (num < 0) // 映射区域是0-9
            {
                int temp = -num;
                temp /= rate;
                temp %= 10;
                bucket[temp].emplace_back(num);
            }
            else
            {
                int temp = num;
                temp /= rate;
                temp %= 10;
                bucket[temp+10].emplace_back(num);
            }
        }
        int index = 0;
        for (int j = 9; j >= 0; --j)
        {
            for (auto& num: bucket[j]) data[index++] = num;
        }
        for (int j = 10; j < 20; ++j)
        {
            for (auto& num: bucket[j]) data[index++] = num;
        }
        bucket.assign(20, vector<int>());
        rate *= 10;
    }
}


int main()
{
    vector<int> v{9, 10, 0, -2, -23, 3, -22, -21, -443, 343, 112};
    radixSort(v);
    for (auto& num: v) cout << num << " ";
    cout << endl;
}