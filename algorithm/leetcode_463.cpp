#include <vector>
#include <iostream>
using namespace std;

class Solution {
public:
    vector<pair<int, int>> posList{make_pair(-1, 0), make_pair(1, 0), make_pair(0, -1), make_pair(0, 1)};
    
    int islandPerimeter(vector<vector<int>>& grid) {
        int row = grid.size();
        int col = grid[0].size();
        
        for (int i = 0; i < row; ++i)
        {
            for (int j = 0; j < col; ++j)
            {
                if (grid[i][j] == 1)
                {
                    std::cout << "grid = " << grid[i][j] << "" << i << "" << j << std::endl;
                    return _dfs(grid, i, j, row, col);
                }
            }    
        }
        return 0;
    }

    int _dfs(vector<vector<int>>& grid, int x, int y, int row, int col)
    {
        if (x < 0 || x >= row || y < 0 || y >= col)
            return 1; // 说明到了边界
        if (grid[x][y] != 1) // 到了边界
            return 1;
        
        // 当前还是水域
        int len = 0;
        grid[x][y] = 0;
        for (auto& [pos_x, pos_y]: posList)
        {
            len += _dfs(grid, x+pos_x, y+pos_y, row, col);
        }
        return len;
    }
};


int main()
{
    Solution s;
    vector<vector<int>> vec{
     {0,1,0,0},
     {1,1,1,0},
     {0,1,0,0},
     {1,1,0,0}
    };
    std::cout << s.islandPerimeter(vec) << std::endl;
}