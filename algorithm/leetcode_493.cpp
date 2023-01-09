#include <bits/stdc++.h>
using namespace std;

class BIT
{
public:
    BIT(int _n): n(_n), tree(n+1) {}

    static constexpr int lowbit(int x)
    {
        return x & (-x);
    }

    void update(int x, int val)
    {
        while (x <= n)
        {
            tree[x] += val;
            x += lowbit(x);
        }
    }

    int query(int x)
    {
        int res = 0;
        while (x > 0)
        {
            res += tree[x];
            x -= lowbit(x);
        }
        return res;
    }

private:
    int n;
    vector<int> tree;
};

class Solution {
public:
    int reversePairs(vector<int>& nums) {
        /**
         * @brief 有两种方式，一种是归并排序，另外一种是树状数组
         * 归并排序：
         * 这种实现方式是在合并之前统计满足条件的数据量，然后再进行合并
         * 树状数组：
         * 树状数组本来是用来解决允许元素变动的区间求和问题（前缀和）
         * 在这种应用下我们需要对原始数据进行排序后的离散化，有两种离散方式：
         * 假设当前有原始数据：vector<int> nums{4, 2, 4, 6}
         * 1）分散式离散化：这种方式不会去重重复值，离散后的数组大小与原大小相同，离散步骤是先排序的{2, 4, 4, 6}，然后按序赋值得：{2, 1, 2, 4}。
         * 2) 紧离散：这种方式是先通过Set去重，因为Set底层实现是红黑树，因此插入后就是有序的了，然后就需要使用unordered_map存储，结果为{(2,1),(4,2),(6,3)}
         */
        // 因为这里的关系是判断2*nums[i]，因此需要同时把该值也插入，否则会出现溢出边界
        // 这里我们不关心数据的具体值，只需要关心大小关系，因此可以将其离散化，这里采用紧离散
        set<long long> mySet; // Set是有序的
        for (auto& num: nums)
        {
            mySet.insert(num);
            mySet.insert((long long)num * 2);
        }
        unordered_map<long long, int> myMap;
        int i = 1;
        for (auto& num: mySet) myMap[num] = i++;

        int res = 0;
        BIT bit(myMap.size()); // 初始化树状数组
        // 因为树状数组的query(k)查询的是前K个数的前缀和，因此我们这里采用后序遍历的方式
        // 比如现有[3, 2, 1]，那么先插入1，query(1)是0，代表在1-1之前没有比他小的数，然后add(1,1)使该位置+1
        // query(2)发现在1-2的前缀和是1，因此有一个结果，然后我们再add(2,1)
        // query(3)在1-3的前缀和1+1=2，因此有两个结果，最后再add(3,1)。
        // 综上总的结果就是3
        // 回到这道题，因为求的是nums[i] > 2*nums[j]，因此需要计算2*nums[j]对应index到最大index之前的前缀和
        // 通常来说遍历方式有两种：一种是从后往前遍历，这种方式是以i为视角，比如计算nums[i] > nums[j]时，query(i)意味着已经统计过的比他小的值。
        // 本题中如果仍是往前遍历以i为视角，那么要计算的是边界就是 nums[i]/2 到 1，直接查询query(nums[i]/2)即可。因为涉及到除法，会有非整数的问题，因此处理起来不方便
        // 因此本题使用从前往后遍历的方式，那么我们的视角就是j了，需要处理的范围是nums[i]*2 到 max，因此我们使用的查询范围就是query(max) - query(nums[i]*2)
        for (int i = 0; i < nums.size(); ++i)
        {
            int right = myMap.size(); // 右边界，最大的index
            int left = myMap[(long long)nums[i] * 2]; // 左边界，2*nums[j]对应的index，注意这里是包含了2*nums[j]而我们题意是大于关系
            
            res += bit.query(right) - bit.query(left); // 因此这里是直接-query(left)，就去掉了2*nums[j]的值
            bit.update(myMap[nums[i]], 1);
        }
        return res;
    }
};


int main()
{
    Solution s;
    vector<int> v{2,4,3,5,1};
    int num = s.reversePairs(v);
    cout << num << endl;
}