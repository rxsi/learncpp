#include <vector>
#include <iostream>
#include <random>

using namespace std;

// void shuffle(vector<int>& nums)
// {
//     int n = nums.size();
//     for (int i = n-1; i >= 0; --i)
//     {
//         int j = rand() % (i + 1); // 注意rand()是伪随机，因此程序重新运行得到的随机结果都是一样的，但是在一次程序内连续调用是不同的，这个在实际应用时要注意，可以使用srand()设置随机种子
//         swap(nums[i], nums[j]); 
//     }
// }

void shuffle(vector<int>& nums, vector<int>& res, int n) // n是要抽取的数据量
{
    int countTimes = 0;

    for (auto num: nums)
    {
        ++countTimes;
        if (countTimes <= n) res.push_back(num);
        else
        {
            int j = rand() % (countTimes + 1);
            if (j <= n) res[j-1] = num; // 随机到了前n个位置，那么可以换入
        }
    }
}

int main()
{
    vector<int> nums{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector<int> res;
    shuffle(nums, res, 5);
    for (auto& num: res) cout << num << " ";
    cout << endl;
}

