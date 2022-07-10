#include <iostream>
#include <queue>
#include <vector>
#include <limits.h>
using namespace std;

#define M INT_MAX/2

/*
在 bellman_ford 算法中，每次都需要遍历所有的边，但是实际上只有在上次的relax中发生了变化的边才会导致其他边发生变化，
因此可以使用队列（先进先出）记录上次变化的边，这样就可以提升速度了。
*/

vector<int> result;

bool spfa(int point, vector<vector<int>>& dist)
{
    int n = dist.size();
    result = vector<int>(n, M);
    // for (int i = 0; i < dist.size(); ++i) result[i] = dist[point][i]; // 因为使用了队列进行记录，因此这里不需要这样做，只需要标记point即可
    result[point] = 0;
    queue<int> Que;
    Que.push(point);
    vector<int> countTimes(n, 0);
    countTimes[point]++;
    while (!Que.empty())
    {
        int p = Que.front();
        Que.pop();
        for (int i = 0; i < n; ++i)
        {
            if (result[p] + dist[p][i] < result[i])
            {
                result[i] = result[p] + dist[p][i];
                countTimes[i]++;
                if (countTimes[i] > n) return false; // 如果存在负权环，则会一直进入队列，因此要统计进入队列的次数，超过总点数则代表存在负权环
                Que.push(i);
            }
        }
    }
    return true;
}

int main()
{
    vector<vector<int>> dist {
        {0, 5, M, 10},
        {M, 0, 3, M},
        {M, M, 0, 1},
        {M, M, M, 0}
    };
    if (spfa(0, dist))
    {
        for (auto& num: result) cout << num << " ";    
    }
    else
    {
        cout << "have ring";
    }
    cout << endl;
} 