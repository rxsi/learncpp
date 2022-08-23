#include<bits/stdc++.h>
using namespace std;
//并查集的应用
vector<int> pre;         //所属集合的头,初始化时为自己
vector<int> intervalSum; //区间和,初始化时为自己
int res = 0;
int find(int x) {
    //查找是否为一个集合
    while (pre[x] != x) x = pre[x];
    return x;
}

void unite(int x, int y) {
    int fx = find(x);
    int fy = find(y);
    if (fx == fy) return;
    pre[fy] = fx;//合并集合
    intervalSum[fx] += intervalSum[fy];//更新最新集合的区间和
    res = max(res, intervalSum[fx]);
}
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    // cin会忽略空格、回车、tab直接读取到一个字符，因此如果要获取上述几个字符则需要使用cin.get() == '\n' cin.get() == ' ' cin.get() == '\t'做判断
    // int n; cin >> n;
    int n = 5;
    int weight[] = {3, 2, 4, 4, 5};//货物的重量
    // for (int i = 0; i < n; ++ i) {
    //     cin >> weight[i];
    // }
    //构造货物取出集合
    vector<int> index{3, 1, 4, 2, 0};
    // for (int i = 0; i < n; ++ i) {
    //     cin >> index[i];
    //     index[i] --;//换为逻辑序号方便操作
    // }
    //初始化pre和intervalSum
    pre.resize(n), intervalSum.resize(n);
    for (int i = 0; i < n; ++ i) {
        pre[i] = i;
        intervalSum[i] = weight[i];
    }
    vector<int> visited(n, false);//此货物是否在集合中
    vector<int> result(n);//存放输出的结果集合
    //逆序的放回获取,获取左右区间的最大值
    for (int i = n - 1; i >= 0; -- i) {
        result[i] = res;
        int j = index[i];
        res = max(res, weight[j]);
        //查看右边是否有获取，进行合并
        if (j < n - 1 && visited[j + 1]) unite(j, j + 1);
        //查看左边是否有货物，进行合并
        if (j > 0 && visited[j - 1]) unite(j - 1, j);
        visited[j] = true;
    }
    for (auto x : result) cout << x << endl;
    return 0;
}