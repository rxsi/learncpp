
#include <vector>
#include <iostream>
using namespace std;

class Solution {
public:
    int findDuplicate(vector<int>& nums) {
        // 如果没有规定不能修改nums,那么可以通过修改下标的方式,当调整下标的数的位置已有相等的数,代表该数是重复的.
        // 当然如果说直接copy一份再执行上面的修改也不是不行....

        vector<int> vec(nums);

        int index = 0;
        int n = vec.size();
        bool check = false;
        while (index < n)
        {
            while (index != vec[index] - 1)
            {
                int temp = vec[vec[index]-1];
                if (temp == vec[index])
                {
                    return temp;
                }
                vec[vec[index]-1] = vec[index];
                vec[index] = temp; 
            }
            index++;
        }
        return 0;
    }
};

int main()
{
    // Solution s;
    // vector<int> vec{1,5,2,8,9,9,3,4,7,6};
    // s.findDuplicate(vec);

    int a = 9;
    int b = 5;
    std::cout << a/b << std::endl; // C++ 的除法默认是舍弃小数点,这会导致在二分法计算时,向left靠拢,即有可能计算出的mid = left,因此不能设置left = mid
}