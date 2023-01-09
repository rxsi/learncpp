#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    int singleNumber(vector<int>& nums) {

        /*
        实现目标：
        1. 如果是第一次出现，那么保存在ones
        2. 如果是第二次出现，那么保存在twos
        3. 如果是第三次出现，那么消失
        */
        int ones = 0, twos = 0;
        for (auto& num: nums)
        {
            ones ^= num & ~twos;
            twos ^= num & ~ones;
        }
        return ones;
    }
};

int main()
{
    Solution s;
    vector<int> nums{30000,500,100,30000,100,30000,100};
    s.singleNumber(nums);
}