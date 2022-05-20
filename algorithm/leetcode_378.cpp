#include <iostream>
#include <vector>
#include <queue>

using namespace std;


class Solution {
public:
    int kthSmallest(vector<vector<int>>& matrix, int k) {
        struct point // 可以在函数内定义结构体，但是不能定义函数
        {
            int val, x, y;
            point(int val, int x, int y) : val(val), x(x), y(y){}
            bool operator>(const point& a) const {return this->val > a.val;}
        };

        priority_queue<point, vector<point>, greater<point>> myQue;
        int n = matrix.size();
        // 归并排序，需要记录x, y, val值，直接用pair吧 pair<x, y>
        for (int i = 0; i < n; ++i) myQue.emplace(matrix[i][0], i, 0);
        for (int i = 0; i < k - 1; ++i)
        {
            auto p = myQue.top();
            myQue.pop();
            if (p.y < n - 1) myQue.emplace(matrix[p.x][p.y + 1], p.x, p.y + 1);
        }
        return myQue.top().val;
    }
};