#include <iostream>
#include <vector>
using namespace std;

/*
单源最短路径算法
相较于dijkstra算法，这个算法可以包含负权边和负权环
在伪代码中返回的是true/false，即用来检测是否有环
时间复杂度是O(V*E)，V是顶点数，E是边的数量

1. 创建源顶点 v 到图中所有顶点的距离的集合 distSet，为图中的所有顶点指定一个距离值，初始均为 Infinite，源顶点距离为 0；
2. 计算最短路径，执行 V - 1 次遍历；
        对于图中的每条边：如果起点 u 的距离 d 加上边的权值 w 小于终点 v 的距离 d，则更新终点 v 的距离值 d；

3. 检测图中是否有负权边形成了环，遍历图中的所有边，计算 u 至 v 的距离，如果对于 v 存在更小的距离，则说明存在环；
*/

#define M INT_MAX/2

pair<bool, vector<int>> bellmanFordFinder(int point, vector<vector<int>>& dist)
{
    vector<int> result(dist.size(), M);
    result[point] = 0;
    for (int i = 1; i < dist.size(); ++i)
    {
        for (auto)
    }
};

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