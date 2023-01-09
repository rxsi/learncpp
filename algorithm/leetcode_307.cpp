#include <bits/stdc++.h>
using namespace std;

// class NumArray {
// public:
//     /*
//     如果区间是固定的那么我们是可以使用前缀和去计算的，但是此处由于区间内的值会动态修改，因此需要通过某些手段去更新之前已经计算好的前缀和，
//     这里就有两种方式：树状数组和线段树
//     树状数组：https://leetcode.cn/circle/discuss/qGREiN/
//     在不使用树状数组时，我们有两种选择，一种是直接使用原始的数组，那么更新某个值是O(1)操作，但是计算范围和就需要O(n)的消耗去遍历；
//     另一种选择是使用前缀和，那么此时我们计算范围和是O(1)的性能，但是更新值则需要O(n)的消耗，因为需要把所有的前缀和都进行更新；
//     而树状数组则是把前缀和划分为了多个小数组，tree[i]代表的是以i为右边界
//     而左边界则是把i转换为二进制然后最低位替换为0，得到的新值就是区间的左边界（所以计算i到j的范围，我们使用query(i) - query(j-1)）
//     在实际计算中，我们会在原始数组大小之前+1，这是因为我们借助了 i & -i 的方式实现上述二进制替换，这个结果返回的是最低位1对应的值
//     所以左边界的计算公式为：(i+1) - lowbit(i+1)
//     以原始数组14个值为例，那么我们会申请tree[15]：
//     tree[0]：0
//     tree[1]：左边界0，右边界1 ，注意在计算左边界是通过(i+1)-lowbit(i+1) = 2 - lowbit(2) = 2 - 2 = 0
//     tree[2]：左2，右2：3 - lowbit(3) = 3 - 1 = 2
//     tree[3]：左0，右3
//     tree[4]：4,4
//     tree[5]：4,5
//     tree[6]：6,6
//     tree[7]：0,7
//     tree[8]：8,8
//     tree[9]：8,9
//     tree[10]：10,10
//     tree[11]：8,11
//     tree[12]：12,12
//     tree[13]：12,13
//     tree[14]：14,14
//     tree[15]：0,15：16-lowbit(16)=16-16=0

//     在上面形成的关系中，含有相同值的下标是有关系的，i+1 + lowbit(i+1)总是等于包含有当前要修改值的下个下标，而i+1 - lowbit(i+1)总是等于包含当前要修改值的上一个下标
//     所以我们在修改某个值或者初始化时，需要把含有该值的所有子数组都更新，第一个需要更新的数组tree[i]（因为它是以i为右边界），后续的坐标通过 i+1+lowbit(i+1) 去计算直到超出了数据边界
//     在上面的更新策略中，我们可知单点更新的数据量等于树的高度，因此时间复杂度是O(logn)
//     当我们去计算某个范围的前缀和（left，right）时，因为树状数组实际上是前缀和的一种实现，因此计算范围前缀和的方式是query(right) - query(left-1)，之所以left-1是因为我们的范围要包含left。

//     如果我们不需要关心元素的具体值，如应用于求前缀和，那么我们就需要进行离散化。关于这部分见493题
    
//     解法2：使用线段树
//     线段树和树状数组的最大应用区别在于树状数组只能是用于处理频繁点更新的前缀和。
//     注意，如果是多次更新后最后求前缀和，那么使用差分数组即可。
//     应用树状数组还可以解决的问题是逆序对问题。
//     而线段树则可以高效的处理区间的更新：
//     原理如下：
//     1. 用数组维护一个二叉树结构，即父节点i那么对应的左子节点为2i,右子节点为2i+1。这个是堆排序一样的思路
//     2. 形成的二叉树的叶子节点对应的是一个长度为1的区间，比如[0,0]，节点的值则需要根据题意来，比如如果是求区间和，那么此处值对应的就是当前区间的累加和，比如是2；
//     而对应的子节点
//     3. 以三个节点应用于求区间和的场景，数组为{1, 2, 3}，那么对应的区间为[0, 2]，由于我们要满足树的结构，因此开辟的数组的容量要为4n（这个有个理论验证，记住这个结论即可）
//     3_1. 一开始需要根据已有数据构建二叉树，对应（build函数），这是一个递归操作，退出的条件是left==right，此时就意味着索引到了叶子节点
    
//      */
//     int n;
//     vector<int> trees;
//     vector<int> delay;
//     vector<int> nums;

//     NumArray(vector<int>& nums) {
//         this->n = nums.size(); // 注意，不要从0开始，因为索引2*i时，0会有异常！！！！树状数组也有同样的做法，要忽略0
//         trees.resize(n*4); // 注意，由于为了构建一颗树，因此这里面会包含很多虚拟节点
//         delay.resize(n*4);
//         this->nums.resize(n+1);
//         for (int i = 1; i < n+1; ++i) this->nums[i] = nums[i-1];
//         buildTrees(1, n, 1); // 构建初始的线段树
//     }

//     void buildTrees(int begin, int end, int index)
//     {
//         /*
//         index 是trees中的节点，begin和end是节点对应的区间范围，初始时index==0，begin对应0，end对应n-1
//         index是trees的下标，初始时指向0，对应的区间是[0,n-1]，后面以index*2 和 index*2+1跳跃至下一个子节点，而区间则是二分
//         */
//         if (begin == end)
//         {
//             trees[index] = nums[begin];
//             return;
//         }
//         int mid = begin + (end - begin) / 2;
//         buildTrees(begin, mid, index * 2);
//         buildTrees(mid + 1, end, index * 2 + 1);
//         trees[index] = trees[index * 2] + trees[index * 2 + 1];
//     }

//     void update(int left, int right, int val, int begin, int end, int index)
//     {
//         /*
//         left是修改区间的左下标，right是右下标，val是修改量。如果是单区间修改，那么left==right
//         index是当前节点，初始时是0，即根节点，线段树每个节点都对应了一个区间，比如叶子节点对应的区间长度为1
//         而根节点对应的区间长度为全部，因此begin和end对应的就是这个区间的长度，初始时为0和n，这个不等价于trees数组的长度
//         而index的左子节点对应的区间为[begin, (begin+end)/2]，右子节点对应[(begin+end)/2+1, end]
//         1. 当left~right完整的包含了begin~end，即index对应的区间完全被包含，也就是现在这个区间内的所有数都要更新val
//         2. 当只包含了一部分，那么就继续递归被包含的那部分

//         懒惰标记：
//         因为如果每次更新某个区间都全部实时更新那么效率不高，因为实际可以进行累加，最后再实际应用到目标节点
//         所以需要在对应节点的父节点处保留一个懒惰标记。
//         比如{1, 2, 3}，我们要更新[0,1]都增加1，那么只需要在节点1和节点2的父节点存储即可。
//         */
//        if (left <= begin && end <= right) // 被完整包围
//        {
//             cout << "begin val = " << trees[index] << endl;
//             trees[index] += (end - begin + 1) * val; // begin~end区间内所有节点都增加val，因此是乘法
//             delay[index] += val; // index的左右子节点有未执行的数据
//             cout << "pack = " << left << " , " << right << " , " << begin << " , " << end << endl;
//             cout << "val = " << trees[index] << " , " << delay[index] << endl; 
//             return;
//        }
//        // 非完整包含，那么只需要计算被包含的部分
//        int mid = begin + (end - begin) / 2;
//        if (delay[index]) // 当前节点的子节点已经有未执行的数据，即本节点的懒惰标记不为0，需要先执行
//        {
//             trees[index * 2] += delay[index] * (mid - begin + 1); // 左子节点，对应的区间是[begin, mid]
//             trees[index * 2 + 1] += delay[index] * (end - mid); // 右子节点，对应的区间是[mid+1, end]
//             delay[index * 2] += delay[index]; // 左子节点继续继承懒惰标记，这意味着如果左子节点不是叶子节点，那么它的子节点也有未计算的数据
//             delay[index * 2 + 1] += delay[index]; 
//             delay[index] = 0; // 已经清空
//        }
//        if (left <= mid) update(left, right, val, begin, mid, index * 2); // 左边有交集
//        if (right > mid) update(left, right, val, mid+1, end, index * 2 + 1); // 右边有交集
//        trees[index] = trees[index * 2] + trees[index * 2 + 1];
//     }

//     int getSum(int left, int right, int begin, int end, int index)
//     {
//         if (left <= begin && end <= right) return trees[index]; // 当前子集被包含了，那么直接返回。这里在一开始时index是0，对应的是最大的区间，因此如果这里匹配上，那么left\right绝对是求最大区间的累加和
//         int mid = begin + (end - begin) / 2;
//         if (delay[index])
//         {
//             trees[index * 2] += delay[index] * (mid - begin + 1); // 左子节点，对应的区间是[begin, mid]
//             trees[index * 2 + 1] += delay[index] * (end - mid); // 右子节点，对应的区间是[mid+1, end]
//             delay[index * 2] += delay[index]; // 左子节点继续继承懒惰标记，这意味着如果左子节点不是叶子节点，那么它的子节点也有未计算的数据
//             delay[index * 2 + 1] += delay[index]; 
//             delay[index] = 0; // 已经清空
//         }
//         int sum = 0;
//         if (left <= mid) sum += getSum(left, right, begin, mid, index * 2);
//         if (right > mid) sum += getSum(left, right, mid + 1, end, index * 2 + 1);
//         return sum;
//     }

//     void update(int index, int val) {
//         update(index+1, index+1, val - nums[index+1], 1, n, 1);
//         nums[index+1] = val;
//     }

//     int sumRange(int left, int right) {
//         return getSum(left+1, right+1, 1, n, 1);
//     }
// };

class NumArray {
public:
    /*
    动态开点，每个节点使用结构体表示
    */
    struct Node
    {
        int left;
        int right;
        int val;
        int lazy;
        Node* leftNode;
        Node* rightNode;
        Node (int left, int right): left(left), right(right), val(0), lazy(0), leftNode(nullptr), rightNode(nullptr){}
    };

    void update(Node* node, int left, int right, int val)
    {
        cout << "left = " << left << " , " << right << " , " << val << endl;
        if (node->left > right || node->right < left) return; // 一般不会到这里，超过了范围了
        if (node->left >= left && node->right <= right) // 在范围内
        {
            node->val += (node->right - node->left + 1) * val;
            node->lazy += val;
            return;
        }
        // 动态开点
        cout << "in lazy" << endl;
        lazyCreate(node);
        cout << "out lazy" << endl;
        // 懒惰下推
        pushDown(node);
        // 更新左右子树
        update(node->leftNode, left, right, val);
        update(node->rightNode, left, right, val);
        // 上传结果
        pushUp(node);
    }

    void lazyCreate(Node* node)
    {
        int mid = node->left + (node->right - node->left) / 2;
        if (node->leftNode == nullptr) node->leftNode = new Node(node->left, mid);
        if (node->rightNode == nullptr) node->rightNode = new Node(mid + 1, node->right);
    }

    void pushDown(Node* node)
    {
        cout << "downs" << endl;
        if (!node->lazy) return;
        node->leftNode->lazy += node->lazy;
        node->rightNode->lazy += node->lazy;
        cout <<"123" << endl;
        node->leftNode->val += (node->leftNode->right - node->leftNode->left + 1) * node->val;
        node->rightNode->val += (node->rightNode->right - node->rightNode->left + 1) * node->val;
        node->lazy = 0;
    }

    void pushUp(Node* node)
    {
        node->val = node->leftNode->val + node->rightNode->val;
    }

    int query(Node* node, int left, int right)
    {
        if (left <= node->left && right >= node->right) return node->val;
        // 动态开点
        lazyCreate(node);
        // 懒惰下推
        pushDown(node);
        int mid = node->left + (node->right - node->left) / 2;
        int sum = 0;
        if (left <= mid) sum += query(node->leftNode, left, right);
        if (right > mid) sum += query(node->rightNode, left, right);
        return sum;
    }

    Node* root;
    vector<int> nums;
    int n;

    NumArray(vector<int>& nums) {
        // root = new Node(0, 1e9); // 初始时上限可以设置为较大的值
        this->n = nums.size();
        this->nums = nums;
        this->root = new Node(0, n*4); // 设置为4*n
        cout << "ok" << endl;
        for (int i = 0; i < n; ++i) update(root, i, i, nums[i]);
        cout << "init" << endl;
    }

    void update(int index, int val) {
        update(root, index, index, val - nums[index]);
        nums[index] = val;
    }

    int sumRange(int left, int right) {
        return query(root, left, right);
    }
};

int main()
{
    vector<int> v{1, 3, 5};
    NumArray n(v);
    n.sumRange(0, 2);
    n.update(1, 2);
    n.sumRange(0, 2);
}