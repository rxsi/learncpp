// #include <iostream>
// #include <bits/stdc++.h>
// using namespace std;

// // long long backtrace(vector<int>& students, int index, int d, long long temp_res, int k, long long res)
// // {
// //     if (k == 0)
// //     {
// //         res = max(res, temp_res);
// //         return;
// //     }
// //     int n = students.size();
// //     for (int i = index+1; i < min(n, index+d+1); ++i)
// //     {
// //         if (students[i] == 0)
// //         {
// //             res = max(res, 0LL);
// //             continue;
// //         }
// //         backtrace(students, i, d, temp_res * students[i], k-1);
// //     }
// // }

// int main() {
//     ios::sync_with_stdio(false);
//     cin.tie(nullptr);
//     // int n = 36;
//     // vector<int> students{
//     //     7, -15, 31, 49, -44, 35, 44, -47, -23, 15, -11, 10, -21, 10, -13, 0, -20, -36, 22, -13, -39, -39, -31, -13, -27, -43, -6, 40, 5, -47, 35, -8, 24, -31, -24, -1 
//     // };
//     int n;
//     while (cin >> n) { // 注意 while 处理多个 case
//         vector<int> students(n);
//         for (int i = 0; i < n; ++i) cin >> students[i];
//         int k, d;

//     // int k = 3;
//     // int d = 31;
//         // cin >> k >> d;
//         // 动态规划
//         /* 最后的状态是dp[i][k] 代表选了k个人，最后第k个人是i，前面的人构成了k-1个人
//         因为当students[i]为负时，需要前面的最小值，所以要么使用pair结构，要么就需要开辟一个新数组g用以存储最小值。
//         从性能上来说，使用数组g作为存储会高效的，因为在遍历的过程需要频繁的修改dp数组的内容，即需要生成pair和释放pair
//         从状态的定义可知，后面的依赖于前面的，因此for循环是从小到大
//         */
//         vector<vector<int>> maxDp(n+1, vector<int>(k+1)); // 存放大值
//         vector<vector<int>> minDp(n+1, vector<int>(k+1)); // 存放小值
//         for (int i = 1; i <= n; ++i)
//         {
//             maxDp[i][1] = students[i-1];
//             minDp[i][1] = students[i-1]; 
//             for (int j = 2; j <= k; ++j)
//             {
//                 for (int left = i-1; left >= i - d && left > 0; --left)
//                 {
//                     maxDp[i][j] = max(maxDp[i][j], max(maxDp[left][j-1] * students[i-1], minDp[left][j-1] * students[i-1]));
//                     minDp[i][j] = min(minDp[i][j], min(maxDp[left][j-1] * students[i-1], minDp[left][j-1] * students[i-1]));
//                 }
//             }
//         }
//         printf("%d\n", maxDp[n][k]);
//     }
//     return 0;
// }
// // 64 位输出请用 printf("%lld")

// /*
// 36
// 7 -15 31 49 -44 35 44 -47 -23 15 -11 10 -21 10 -13 0 -20 -36 22 -13 -39 -39 -31 -13 -27 -43 -6 40 5 -47 35 -8 24 -31 -24 -1 
// 3 31

// 预期：108241
// 实际：2303
// */

#include <iostream>
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    // int n = 8;
    // vector<int> students{
    //     -41, -5, -10, -31, -44, -16, -3, -33
    // };
    int n;
    while (cin >> n) { // 注意 while 处理多个 case
        vector<int> students(n);
        for (int i = 0; i < n; ++i) cin >> students[i];
        int k, d;

    // int k = 6;
    // int d = 3;
        cin >> k >> d;
        // 动态规划
        /* 最后的状态是dp[i][k] 代表选了k个人，最后第k个人是i，前面的人构成了k-1个人
        因为当students[i]为负时，需要前面的最小值，所以要么使用pair结构，要么就需要开辟一个新数组g用以存储最小值。
        从性能上来说，使用数组g作为存储会高效的，因为在遍历的过程需要频繁的修改dp数组的内容，即需要生成pair和释放pair
        从状态的定义可知，后面的依赖于前面的，因此for循环是从小到大
        */
        vector<vector<int>> maxDp(n+1, vector<int>(k+1)); // 存放大值
        vector<vector<int>> minDp(n+1, vector<int>(k+1)); // 存放小值
        int res = INT_MIN;
        for (int i = 1; i <= n; ++i)
        {
            maxDp[i][1] = students[i-1];
            minDp[i][1] = students[i-1];
            for (int j = 2; j <= k; ++j)
            {
                if (j > i)
                {
                    maxDp[i][j] = INT_MIN;
                    minDp[i][j] = INT_MAX;
                    continue;
                }
                int leftMax = INT_MIN;
                int leftMin = INT_MAX;
                for (int left = i-1; left >= i - d && left > 0; --left)
                {
                    leftMax = max(leftMax, maxDp[left][j-1]);
                    leftMin = min(leftMin, minDp[left][j-1]);
                }
                cout << "i = " << i << " j = " << j << endl;
                cout << "leftMax = " << leftMax << endl;
                cout << "leftMin = " << leftMin << endl;
                if (students[i-1] >= 0)
                {
                    maxDp[i][j] = leftMax * students[i-1];
                    minDp[i][j] = leftMin * students[i-1];
                }
                else
                {
                    maxDp[i][j] = leftMin * students[i-1];
                    minDp[i][j] = leftMax * students[i-1];
                }
            }
            res = max(res, maxDp[i][k]);
        }
        printf("%d\n", res);
    }
    return 0;
}

// 64 位输出请用 printf("%lld")