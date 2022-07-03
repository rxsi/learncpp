#include <iostream>
#include <vector>
#include <queue>
using namespace std;

#define M 99999

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