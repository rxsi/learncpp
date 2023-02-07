#include <bits/stdc++.h>
using namespace std;

class Solution {
private:
    // vector<int> path;
    vector<vector<int>> res;
    unordered_set<int> visited;

public:
    vector<vector<int>> permute(vector<int>& nums) {
        _backtrace(nums);
        return res;
    }

    void _backtrace(vector<int>& nums)
    {
        if (visited.size() == nums.size())
        {
            res.emplace_back(visited.begin(), visited.end());
            return;
        }
        for (auto& num: nums)
        {
            if (visited.count(num)) continue;
            visited.insert(num);
            _backtrace(nums);
            visited.erase(num);
        }
    }
};

int main()
{
    Solution s;
    vector<int> nums{1, 2, 3};
    s.permute(nums);
}