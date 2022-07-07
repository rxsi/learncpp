#include <limits.h>
#include <memory>
#include <vector>
#include <queue>
#include <math.h>
#include <unordered_map>
#include <iostream>

using namespace std;

class aStar
{
public:
    using Map = vector<vector<int>>;
    vector<pair<int, int>> search(const Map&, int, int, int, int);
private:
    bool _isValid(const Map&, int, int);
    int _calH(int, int, int, int);
    int _calG(const Map&, int, int, int);
    static int D; // 直线运动的代价
    static int DD; // 斜线运动的代价
    static vector<pair<int, int>> path;
    int _maxX; // 当前地图X的最大值
    int _maxY; // 当前地图Y的最大值

    struct Node
    {
        int x; // x轴位置
        int y; // y轴位置
        int g; // g值
        int h; // h值
        Node* parent; // 父节点
        bool close; // 是否已经在close中，true是，false否
        Node(int x, int y): x(x), y(y), close(false), parent(nullptr){}
    };
    struct cmp
    {
        bool operator()(const Node* node1, const Node* node2)
        {
            return node1->g + node1->h > node2->g + node2->h;
        }
    };
};

int aStar::D = 10;
int aStar::DD = 14;
vector<pair<int, int>> aStar::path{
    /*
    搜索节点周围的点
    按照八个方位搜索
    (x-1,y-1)(x-1,y)(x-1,y+1)
    (x  ,y-1)(x  ,y)(x  ,y+1)
    (x+1,y-1)(x+1,y)(x+1,y+1)
    */
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} // 前四个方向是直线，后四个是斜线
};

bool aStar::_isValid(const Map& map, int x, int y)
{
    return 0 <= x < _maxX && 0 <= y < _maxY && map[x][y] > -1;
}

int aStar::_calH(int current_x, int current_y, int end_x, int end_y)
{
    int dx = abs(current_x - end_x);
    int dy = abs(current_y - end_y);
    /*
    三种计算方式：
    1. 曼哈顿距离，适合只能直线运动的场景：(dx + dy) * D
    2. 对角距离，适合直线+45度角运动的场景：(dx + dy) + (DD - 2*D) * min(dx, dy)
    3. 欧几里得距离：适合任意角度运行，有sqrt操作，相对较慢，开方可以使用别的方式优化：D * sqrt(dx * dx + dy * dy)
    */
    return D * sqrt(dx * dx + dy * dy);
}

int aStar::_calG(const Map& map, int dest_x, int dest_y, int weight)
{
    /*
    如果是直线运动，则结果会是 D * 目标坐标点的地形权重，比如沙漠权重高点，平地权重就低点，即优先走平地
    */
    return weight * map[dest_x][dest_y]; 
}

vector<pair<int, int>> aStar::search(const Map& map, int start_x, int start_y, int end_x, int end_y)
{
    _maxX = map.size();
    _maxY = map[0].size();
    if (!_isValid(map, start_x, start_y) or !_isValid(map, end_x, end_y)) // 起始/终点坐标异常
    {
        return {};
    }
    unordered_map<int, Node*> nodeMap;
    Node* startNode = new Node(start_x, start_y);
    nodeMap[startNode->x + _maxX * startNode->y] = startNode; // x + maxX * y 这种方式可以保证x和y计算出来的值是唯一的
    startNode->g = 0;
    startNode->h = _calH(startNode->x, startNode->y, end_x, end_y);
    priority_queue<Node*, vector<Node*>, cmp> Que; // 使用 priority_queue 会有重复插入的问题，因此需要使用close字段进行判断，和dijkstra算法处理相似
    Que.emplace(startNode);
    vector<pair<int, int>> ret;
    while (!Que.empty())
    {
        Node* parentNode = Que.top();
        Que.pop();
        if (parentNode->close) continue;
        parentNode->close = true;
        if (parentNode->x == end_x && parentNode->y == end_y)
        {
            while (parentNode->parent != nullptr)
            {
                ret.emplace_back(parentNode->x, parentNode->y);
                parentNode = parentNode->parent;
            }
            ret.emplace_back(parentNode->x, parentNode->y);
            // 内存释放
            for (auto iter = nodeMap.begin(); iter != nodeMap.end(); ++iter) delete iter->second;
            return ret;
        }
        for (int i = 0; i < path.size(); ++i)
        {
            int new_x = path[i].first + parentNode->x;
            int new_y = path[i].second + parentNode->y;
            if (!_isValid(map, new_x, new_y)) continue;
            int idx = new_x + _maxX * new_y;
            if (nodeMap.find(idx) != nodeMap.end()) // 已经遍历过
            {
                Node* childNode = nodeMap[idx];
                if (childNode->close) continue;
                int new_g = _calG(map, new_x, new_y, i < 4 ? D : DD); // 直线 or 斜线，对于重复的节点，这里就会由于父节点位置的不同产生不同的值
                int new_h = _calH(new_x, new_y, end_x, end_y);
                if (new_g + new_h < childNode->g + childNode->h) // 有更小的权重，这里会造成重复插入，上面有过滤，因此无妨
                {
                    childNode->g = new_g;
                    childNode->h = new_h;
                    childNode->parent = parentNode;
                    Que.emplace(childNode);
                }
            }
            else
            {
                Node* childNode = new Node(new_x, new_y);
                nodeMap[idx] = childNode;
                int new_g = _calG(map, new_x, new_y, i < 4 ? D : DD); // 直线 or 斜线
                int new_h = _calH(new_x, new_y, end_x, end_y);
                childNode->g = new_g;
                childNode->h = new_h;
                childNode->parent = parentNode;
                Que.emplace(childNode);
            }
        }
    }
    return {}; // 没有找到路径
}


int main()
{
    // vector<vector<int>> map{ // -1表示阻挡,其他数值表示当前点的权重值,值越大代表地形越"难走",优先级越低
    //     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    //     {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    //     {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1},
    //     {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    //     {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    //     {1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    //     {1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1},
    //     {1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
    //     {1, 0, 0, 1, 0, 0, 0, 0 ,0, 1, 0 ,0, 1, 0, 0, 0, 0, 1, 0, 1},
    //     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    // };
    vector<vector<int>> map{ // -1表示阻挡,其他数值表示当前点的权重值,值越大代表地形越"难走",优先级越低
        {-1,    -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1},
        {-1,    0,      5,      0,      0,      0,      1,      0,      -1,     -1},
        {-1,    0,      10,      0,      0,      0,      1,      0,      -1,     -1},
        {-1,    10,      -1,     -1,     -1,     -1,     -1,     -1,     0,      -1},
        {-1,    0,      0,      0,      0,      0,      1,      0,      -1,     -1},
        {-1,    0,      0,      0,      0,      0,      1,      1,      0,      -1},
        {-1,    -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1}
    };
    aStar a;
    auto path = a.search(map, 1, 1, 5, 8);
    for (int i = path.size()-1; i >= 0; --i)
    {
        cout << "(" << path[i].first << ", " << path[i].second << ")";
        if (i != 0) cout << " => ";
    }
    cout << endl;
}