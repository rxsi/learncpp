#include <iostream>
#include <vector>
#include <queue>
using namespace std;

/*
如果使用Python实现将会更加容易，因为Python可以直接更新优先队列中的数，然后再调整堆即可
该算法是无法处理负权边的，因为我们每次选点都是基于：当前未找到的最短路的点中，距离源节点最近的点都是已经无法继续松弛的了
而如果存在负权边，比如[[0, 2, 3], [M, 0, M], [M, -2, 0]]，如果根据dijkstra的规则，首先计算的点将会是1，而其实0 -> 2 -> 1才是真正最短的距离
当前算法自然也就不可以处理负权环的问题了。
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