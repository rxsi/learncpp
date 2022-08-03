/*
平方数组去重
*/
#include <iostream>
#include <vector>
using namespace std;

int diffSqureNum(const vector<int>& nums)
{
    int left = 0, right = nums.size()-1;
    int res = 0;
    while (left <= right)
    {
        if (nums[left] + nums[right] == 0)
        {
            res++;
            int temp = nums[left];
            while (left <= right && nums[left] == temp) left++;
            while (left <= right && nums[right] == -temp) right--;
        }
        else if (nums[left] + nums[right] > 0)
        {
            res++;
            int temp = nums[right];
            while (left <= right && nums[right] == temp) right--;
        }
        else
        {
            res++;
            int temp = nums[left];
            while (left <= right && nums[left] == temp) left++;
        }
    }
    return res;
}

int main()
{
    vector<int> v{-2, -1, 0, 1, 2, 3, 4};
    cout << diffSqureNum(v) << endl;
}