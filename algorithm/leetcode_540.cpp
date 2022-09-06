#include <vector>
#include <iostream>
using namespace std;

class Solution {
public:
    int singleNonDuplicate(vector<int>& nums) {
        int n = nums.size(); // 总数为奇数
        int left = 0, right = n; // 左闭右开
        while (left < right)
        {
            int mid = left + (right - left) / 2;
            cout << mid << " " << left << " " << right << endl;
            if (mid - 1 >= 0 && mid + 1 < n)
            {
                if (nums[mid] != nums[mid-1] && nums[mid] != nums[mid+1]) return nums[mid];
                if (nums[mid] == nums[mid-1]) // 1, 1, 2, 3, 3, 4, 4, 5, 5
                {
                    if ((mid - left - 1) & 1) right = mid - 1; // 结果为1，说明是奇数，那么那个值在左边
                    else left = mid + 1; //说明是偶数个，值在右边 
                }
                else // 1, 1, 3, 3, 4, 4, 5, 5, 6
                {
                    if ((mid - left + 1) & 1) left = mid + 2; // 值在右边
                    else right = mid; 
                }
            }
            else if (mid - 1 == 0)
            {
                if (nums[mid] != nums[mid+1]) return nums[mid];
                else left = mid + 2;
            }
            else
            {
                if (nums[mid] != nums[mid-1]) return nums[mid];
                else right = mid - 1;
            }
        }
        return nums[left];
    }
};

int main()
{
    vector<int> v{1, 1, 2, 3, 3, 4, 4, 8, 8};
    Solution s;
    s.singleNonDuplicate(v);
}