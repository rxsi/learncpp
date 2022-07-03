#include <iostream>
#include <vector>
using namespace std;

#define M 99999


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