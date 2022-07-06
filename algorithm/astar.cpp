#include <limits.h>
#include <memory>
#include <vector>
#include <queue>
#include <math.h>

using namespace std;

#define M INT_MAX / 2

/*
节点信息
*/
class Node 
{
public:
    Node(int, int);
    void setG(int);
    void setH(int);
    void setParent(shared_ptr<Node>);
    int getX() const;
    int getY() const;
    int getF() const;
    int getG() const;
    bool isClose() const;
    int setWeight(int);
    void setClose();
    shared_ptr<Node> getParent();

private:
    int x;
    int y;
    shared_ptr<Node> parent; // 指向父对象
    int g;
    int h;
    int weight;
    bool close; // 当为1时表示已经计算处最小,不再计算
};

Node::Node(int x, int y): x(x), y(y){}
void Node::setG(int g)
{
    g = g;
}

void Node::setH(int h)
{
    h = h;
}

void Node::setParent(shared_ptr<Node> p)
{
    parent = p;
}

inline int Node::getX() const
{
    return x;
}

inline int Node::getY() const
{
    return y;
}

inline int Node::getF() const
{
    return x + y;
}

inline int Node::getG() const
{
    return g;
}

inline bool Node::isClose() const
{
    return isClose;
}

inline int Node::setWeight(int weight)
{
    weight = weight;
}

inline void Node::setClose()
{
    close = true;
}

shared_ptr<Node> Node::getParent()
{
    return parent;
}


class aStar
{
public:
    using NodePtr = shared_ptr<Node>;
    vector<pair<int, int>> search(vector<vector<int>>&, int, int, int, int);

private:
    inline bool _isValid(vector<vector<int>>&, int, int) const;
    void _searchNear(const NodePtr&, const NodePtr&) const;
    void _searchOneNode(Node&) const;
    int _calG(vector<vector<int>>& map, shared_ptr<Node>& node1, shared_ptr<Node>& node2) const;
    int _calH(const NodePtr&, const NodePtr&) const;
    bool _isEndNode(const NodePtr&, const NodePtr&) const;

private:
    int row;
    int col;
    static int D;
    static int DD;
    struct cmp
    {
        bool operator()(NodePtr& m, NodePtr& n)
        {
            return m->getF() < n->getF();
        }
    };
    
    // vector<shared_ptr>;
};

int aStar::D = 10;
int aStar::DD = 14;

// int aStar::_calG(shared_ptr<Node>& node1, shared_ptr<Node>& node2) const
// {
//     if (abs(node1->getX() - node2->getX() == 1 && abs(node1->getY() - node2->getY()) == 1)
//     {
//         return sqrt(2) * Node.Weight
//     }

// }

int aStar::_calH(const NodePtr& node1, const NodePtr& node2) const
{
    int dx = abs(node1->getX() - node2->getX());
    int dy = abs(node2->getY() - node2->getY());
    return (dx + dy) + (DD - 2*D) * min(dx, dy); // 对角距离
}

inline bool aStar::_isEndNode(const NodePtr& node1, const NodePtr& node2) const
{
    return node1->getX() == node2->getX() && node1->getY() == node2->getY();
}

inline bool aStar::_isValid(vector<vector<int>>& map, int x, int y) const
{
    return 0 <= x && x < this->row && 0 <= y && y < this->col && map[x][y] != -1;
}

vector<pair<int, int>> aStar::search(vector<vector<int>>& map, int start_x, int start_y, int end_x, int end_y)
{
    this->row = map.size();
    this->col = map[0].size();
    if (_isValid(map, start_x, start_y) or _isValid(map, end_x, end_y))
    {
        return {};
    }
    auto startNode = make_shared<Node>(start_x, start_y);
    startNode->setWeight(map[start_x][start_y]);
    auto endNode = make_shared<Node>(end_x, end_y);
    endNode->setWeight(map[end_x][end_y]);
    priority_queue<NodePtr, vector<NodePtr>, cmp> myQue;
    startNode->setG(0);
    startNode->setH(_calH(startNode, endNode));
    myQue.emplace(startNode);
    vector<NodePtr> closeList;
    vector<pair<int, int>> pathList;
    while (!myQue.empty())
    {
        auto currentNode = myQue.top();
        myQue.pop();
        if (_isEndNode(currentNode, endNode))
        {
            while (1)
            {
                pathList.emplace_back(currentNode->getX(), currentNode->getY());
                if (currentNode->getParent())
            }
        }
        if (currentNode->isClose()) continue;
        p->setClose();
        _searchNear(currentNode, endNode, myQue, )
    }

}

inline void aStar::_searchNear(const NodePtr& currentNode, const NodePtr& endNode)
{
    static vector<pair<int, int>> direct{
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}
    };

    for (auto& p: direct)
    {
        
    }
}




int main()
{
    auto startNode = make_shared<Node>(new Node(1,1));
    auto endNode = make_shared<Node>(new Node(10, 10));
    vector<vector<int>> map{ // -1表示阻挡,其他数值表示当前点的权重值,值越大代表地形越"难走",优先级越低
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 0 ,0, 1, 0 ,0, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    aStar a;
    auto path = a.search(map, 1, 1, 10, 10);
}