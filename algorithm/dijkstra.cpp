#include <iostream>
#include <vector>
#include <queue>
#include <limits.h>
using namespace std;

/*
如果使用Python实现将会更加容易，因为Python可以直接更新优先队列中的数，然后再调整堆即可
该算法是无法处理负权边的，因为我们每次选点都是基于：当前未找到的最短路的点中，距离源节点最近的点都是已经无法继续松弛的了
而如果存在负权边，比如[[0, 2, 3], [M, 0, M], [M, -2, 0]]，如果根据dijkstra的规则，首先计算的点将会是1，而其实0 -> 2 -> 1才是真正最短的距离
当前算法自然也就不可以处理负权环的问题了。

根据图的类型，分为两种方式：
1. 如果图是邻接矩阵(本题就是邻接矩阵)，一般是稠密图，也就是计算源节点到其他节点的距离都需要遍历，因此使用优先队列是没有什么意义的，都是O(n2)，如果是Python那种就有意义
2.如果图是邻接表，一般是稀疏图，因此使用优先队列就可以优化，因为我们可以根据点直接拿到他能到达的所有点，一般这么定义：vector<vector<pair<int, int>>> path; path里面包含的就是key能够到达的所有点的列表
见https://leetcode.cn/problems/network-delay-time/

采用贪心算法的思想，时间复杂度是O(V*V），V是顶点数

*/

#define M INT_MAX/2


vector<int> dijkstraFinder(int point, vector<vector<int>>& dist)
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> Priority_Queue;
    vector<int> result(dist.size(), M);
    vector<bool> visited(dist.size(), false);
    Priority_Queue.emplace(0, point);
    while (!Priority_Queue.empty())
    {
        auto p = Priority_Queue.top();
        Priority_Queue.pop();
        if (p.first > result[p.second]) continue; // 因为这种做法可能会加入重复的点，因此需要忽略那些不合适的值
        result[p.second] = p.first;
        visited[p.second] = true;
        for (int i = 0; i < dist.size(); ++i)
        {
            if (visited[i]) continue;
            if (dist[p.second][i] + p.first <= dist[point][i])
            {
                Priority_Queue.emplace(dist[p.second][i] + p.first, i);
            }
        }
    }
    return result;
}

int main()
{
    vector<vector<int>> dist {
        {0, 5, M, 10},
        {M, 0, 3, M},
        {M, M, 0, 1},
        {M, M, M, 0}
    };
    vector<int> ret = dijkstraFinder(0, dist);
    for (auto& num: ret) cout << num << " ";
    cout << endl;
} 