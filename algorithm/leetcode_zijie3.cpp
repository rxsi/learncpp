/*
给定一个n和m，n代表了任务总数，m代表了总消耗

每个任务有两个参数：需要的消耗值，获利

要求在完成2个任务，且消耗不超过m的情况下的最大获利

采用二分法计算，固定一个值，注意要保证分数大于已有最高分数，同时消耗要小于最高消耗

*/
#include <iostream>
#include <vector>
#include <ios>
#include <algorithm>

using namespace std;

struct myCmp
{
    bool operator()(const vector<int>& v1, const vector<int>& v2)
    {
        return v1[1] < v2[1];
    }
};

int binarySearch(int left, int right, vector<int>& task, int m, int maxScore, vector<vector<int>>& tasks)
{
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        int newSumScore = task[1] + tasks[mid][1];
        if (newSumScore >= maxScore && task[0] + tasks[mid][0] <= m)
        {
            maxScore = newSumScore;
            left = mid + 1;
        }
        else if (newSumScore >= maxScore) // 这种情况需要分成两种情况分别计算
        {
            maxScore = max(maxScore, binarySearch(left, mid-1, task, m, maxScore, tasks));
            maxScore = max(maxScore, binarySearch(mid+1, right, task, m, maxScore, tasks));
            break;
        }
        else
        {
            left = mid + 1;
        }
    }
    return maxScore;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    // int n, m;
    // cin >> n >> m;
    int n = 6, m = 6;

    // vector<vector<int>> tasks(n, vector<int>(2));
    // for (int i = 0; i < n; ++i)
    // {
    //     cin >> tasks[i][0] >> tasks[i][1];
    // }
    vector<vector<int>> tasks{{7, 8}, {1, 1}, {4, 3}, {6, 4}, {7, 5}, {5, 6}};

    sort(tasks.begin(), tasks.end(), myCmp()); // 根据获利进行排序
    // 使用二分法进行计算
    int maxScore = 0;
    for (int i = 0; i < tasks.size(); ++i)
    {
        if (tasks[i][0] >= m) continue; // 超过了总消耗
        maxScore = max(maxScore, binarySearch(i+1, tasks.size()-1, tasks[i], m, maxScore, tasks));
    }

    cout << maxScore << endl;
    return 0;
}