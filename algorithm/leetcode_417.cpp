#include <vector>
using namespace std;
#include <unordered_map>
#include <unordered_set>
#include <iostream>

class Solution {
public:
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& heights) {
        int m = heights.size();
        int n = heights[0].size();
        unordered_map<int, vector<int>> result_cache;
        vector<vector<int>> res;
        unordered_set<int> visited;
        for (int i = 0; i < m; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                int pos = i * (n+1) + j;
                if (result_cache.find(pos) != result_cache.end())
                {
                    if (result_cache[pos][0] && result_cache[pos][1]) res.push_back({i, j});
                    else continue;
                }
                else
                {
                    vector<int> temp(2);
                    dfs(result_cache, i, j, m, n, temp, visited, heights);
                    if (temp[0] && temp[1]) res.push_back({i, j});
                    result_cache[pos] = temp;
                    cout << pos << " " << i << " " << j << endl;
                }
            }
        }
        return res;
    }

    void dfs(unordered_map<int, vector<int>>& result_cache, int x, int y, int m, int n, vector<int>& temp, unordered_set<int>& visited, vector<vector<int>>& heights)
    {
        if (temp[0] && temp[1]) return;
        if (x == 0 || y == 0) temp[0] = 1;
        if (x == m-1 || y == n-1) temp[1] = 1;
        if (x < 0 || x > m-1 || y < 0 || y > n-1) return;
        int pos = x*(n+1)+y;
        cout << "pos = " << pos << " " << x << " " << y << endl;
        cout << "visited ";
        for (auto& v: visited) cout << v << " ";
        cout << endl; 
        for (auto& kv : pos_list)
        {
            int newX = x + kv[0], newY = y + kv[1];
            int newPos = newX * (n+1) + newY;
            if (newX < 0 || newX > m-1 || newY < 0 || newY > n-1 || visited.find(newPos) != visited.end()) continue;
            if (heights[x][y] >= heights[newX][newY])
            {
                if (result_cache.find(newPos) != result_cache.end())
                {
                    if (result_cache[newPos][0]) temp[0] = 1;
                    if (result_cache[newPos][1]) temp[1] = 1;
                    if (temp[0] && temp[1]) return;
                    continue;
                }
                else
                {
                    visited.insert(newPos);
                    dfs(result_cache, newX, newY, m, n, temp, visited, heights);
                    visited.erase(newPos);
                }
            }
        }
    }

private:
    vector<vector<int>> pos_list{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
};


int main()
{
    Solution s;
    vector<vector<int>> vec{{1,2,2,3,5},{3,2,3,4,4},{2,4,5,3,1},{6,7,1,4,5},{5,1,1,2,4}};
    vector<vector<int>> res;
    res = s.pacificAtlantic(vec);
    cout << "res ";
    for (auto& vec: res)
    {
        cout << vec[0] << " " << vec[1] << endl;
    }
}