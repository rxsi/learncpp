#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};
 
class Solution {
public:
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        if (preorder.empty() || inorder.empty()) return NULL;
        int root_val = preorder[0];
        TreeNode* root = new TreeNode(root_val);
        auto iter = find(inorder.begin(), inorder.end(), root_val); // 首先是find函数，要在algorithm头文件下
        int len = iter - inorder.begin();
        vector<int> leftPreorder = vector<int>(preorder.begin()+1, preorder.begin()+len+1); // 如果前iter和后iter相等，则截取到的是空，后iter = 前iter+1则代表的是截取前iter所在位置的元素，注意后tier不可小于前iter，否则报错
        vector<int> rightPreorder = vector<int>(preorder.begin()+len+1, preorder.end());
        vector<int> leftInorder = vector<int>(inorder.begin(), inorder.begin()+len);
        vector<int> rightInorder = vector<int>(inorder.begin()+len+1, inorder.end()); 
        root->left = buildTree(leftPreorder, leftInorder);
        root->right = buildTree(rightPreorder, rightInorder);
        return root;
    }
};

int main()
{
    vector<int> a{3, 9, 20, 15, 7};
    vector<int> b{9, 3, 15, 20, 7};
    Solution s;
    TreeNode* root = s.buildTree(a, b);
    cout << root->val << endl;
}