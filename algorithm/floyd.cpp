#include <iostream>
#include <vector>
#include <limits.h>
using namespace std;

#define M INT_MAX/2

/*
该算法可以计算有负权的边，因为他对于每一组点的计算都是通过遍历所有点之后才确定的，因此有负权的边最终也会被正确的计算
但是，他不可以计算有负权环的图，因为每次都走负权环，则每次的路径都在减少，最终是无穷小
*/

vector<vector<int>> floydFinder(vector<vector<int>>& dist)
{
    int n = dist.size();
    vector<vector<int>> dp(dist);
    vector<vector<int>> path(n, vector<int>(n, -1));
    for (int k = 0; k < n; ++k)
    {
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (dp[i][j] > dp[i][k] + dp[k][j])
                {
                    dp[i][j] = dp[i][k] + dp[k][j];
                    path[i][j] = k;
                }
            }   
        }
    }
    return path;
}

void findPath(vector<vector<int>>& path, int start, int end, vector<int>& route)
{
    if (path[start][end] == -1)
    {
        if (route.empty())
        {
            route.push_back(start);
            route.push_back(end);
        }
        else
        {
            route.push_back(end);
        }
        return;
    }
    else
    {
        int k = path[start][end];
        findPath(path, start, k, route);
        findPath(path, k, end, route);
        return;
    }
}


int main()
{
    vector<vector<int>> dist {
        {0, 5, M, 10},
        {M, 0, 3, M},
        {M, M, 0, 1},
        {M, M, M, 0}
    };
    vector<vector<int>> path = floydFinder(dist);
    vector<int> route;
    findPath(path, 0, 3, route);
    for (auto& i: route)
    {
        cout << i << " ";
    }
    cout << endl;
}