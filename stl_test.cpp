/*
stl容器

关于容器,如果存储的是内建类型,如int\char\float等,那么只是简单的位拷贝,成本可忽略不计

但是如果存储的是自定义类型,那么就需要经过这么一个过程:(使用push_back的情况下)
1. 创建临时对象
2. 将临时对象拷贝到容器内
3. 析构临时对象

如果使用的是emplace_back,则会直接在容器内创建对象,效率更高

当从容器中取出自定义类型对象时( *begin()的方式 ),需要经过拷贝构造,也就是说拿到的也不是同一个对象.当然如果只是用begin指针去操作容器内对象的话,操作的还是同一个对象类

因此存储自定义类型的指针,可以带来更高的性能,但是如果直接存储指针还需要delete,因此最好存储智能指针
而且直接存储自定义类的一个重要问题是当子类被传入时会丢失子类独有的数据,而使用指针则不会有这个问题


*/
#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <queue>
#include <stack>
#include <memory>
#include <forward_list>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <algorithm>
using namespace std;




/*
vector底层是连续的内存段

*/

class Node
{
public:
    Node(int num1, int num2): x(num1), y(num2)
    {
        std::cout << "construct Node" << std::endl;
    }

    ~Node()
    {
        std::cout << "delete Node" << std::endl;
    }

    Node(const Node& node)
    {
        this->x = node.x;
        this->y = node.y;
        std::cout << "copy construct Node" << std::endl;
    }

    int x;
    int y;
};


struct myCompare
{
    bool operator() (Node node1, Node node2) // 这里传进来的node1在序列中的顺序是先于node2的，如果这个顺序是正确的，那么返回true，否则返回false
    {
        if (node1.x == node2.x)
            return node1.y < node2.y;
        return node1.x < node2.x; 
    }
};

bool myCmp(pair<int, int>& m, pair<int, int>& n)
{
    return m.second > n.second; // compare函数的计算方法在于,传入的m本身在n的前面（注意是大顶堆结构）,如果返回true即表示当前排序是正确的,返回false则会重排，注意这里是大顶堆，也就是如果返回true，那么就是符合大顶堆的设置
}

struct myCmp2
{
    bool operator() (pair<int, int>& m, pair<int, int>& n) // 在排序结构中，默认使用的是大顶堆，且m在n之前，如果返回true，那么意味着当前符合大顶堆结构，即最后结果是从大到小排序
    {
        if (m.first == n.first) return m.second > n.second; 
        return m.first > n.first;
    }
};

struct myCmp3
{
    bool operator() (int m, int n)
    {
        return m < n;
    }
};

struct myCmp4 // 这个应用于sort函数，默认是less函数，即从小到大排序，因此如果返回true则是当前的m和n是符合顺序的
{
    bool operator() (int m, int n)
    {
        return m > n;
    }
};

struct pair_hash
{
    template<typename T1, typename T2>
    std::size_t operator() (const pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first); // 这里的{}的意思是实例化一个临时的hash对象,然后调用该对对象的operator()方法.
        auto h2 = std::hash<T2>{}(p.second); // 等价于std::hash<T2>()(p.second), 也等价于std::hash<T2> t; t(p.second);
        return h1 ^ h2;
    }
};



int main(){
    vector<int> vec = {1};
    // for 循环每次都会重新计算，因此不能边遍历边push_back插入数据，会导致一直循环
    // for (auto iter = vec.begin(); iter != vec.end(); ++iter){ 
    //     vec.push_back(*iter+1);
    //     cout << "push_back num = " << *iter+1 << endl;
    // }

    // 使用vec.size()依然会重新计算，因此和直接使用迭代器没有区别
    // for (size_t i = 0; i < vec.size(); ++i){
    //     vec.push_back(i+1);
    //     cout << "push_back num = " << i+1 << endl;
    // }

    // auto iter = vec.begin();
    // while (iter != vec.end()){ // 使用while也一样会造成无限循环
    //     vec.push_back(*iter+1);
    //     cout << "push_back num = " << *iter+1 << endl;
    // }
    /*
    应该优先使用while循环，因为可以处理erase的情况
    每当erase，后续的元素会往前移位，因此for循环将会有异常
    */
//    auto iter = vec.begin();
//    while (iter != vec.end()){
//        if (*iter > 2){
//            iter = vec.erase(iter);
//        } else {
//            ++iter;
//        }
//    }

    /*
    最终做法：
    1. push_back触发扩容时，会导致所有的迭代器都失效，因此不能使用迭代器遍历增加，只能使用下标
    2. erase删除元素之后，当前位置之后的元素都会往前移动，因此迭代器遍历需要保证在删除时不改变迭代器位置
    3. erase只接收迭代器参数，因此使用begin()+index的方式计算
    */
    // int iter = 0; 
    // while (iter < vec.size()){
    //     if (vec[iter] <= 3 && vec[iter] >= 1){
    //         vec.push_back(vec[iter]+10);
    //         ++iter;
    //     } else if (vec[iter] >= 4 && vec[iter] <= 5){
    //         vec.erase(vec.begin()+iter);
    //     } else {
    //         ++iter;
    //     }
    // }

    // for (auto& val : vec){
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;

    // std::cout << "vec_size: " << vec.size() << std::endl;
    // std::cout << "vec_capacity: " << vec.capacity() << std::endl;
    // std::cout << "vec_max_size: " << vec.max_size() << std::endl;

    // vec.assign(2, 1);

    // std::cout << "vec_size: " << vec.size() << std::endl;
    // std::cout << "vec_capacity: " << vec.capacity() << std::endl;
    // std::cout << "vec_max_size: " << vec.max_size() << std::endl;

    // std::list<int> l1 = {1, 2, 3, 4, 5, 6};
    // std::cout << "list_size: " << l1.size() << std::endl;
    // std::cout << "list_max_size: " << l1.max_size() << std::endl;

    // l1.assign(1, 1);
    // std::cout << "list_size: " << l1.size() << std::endl;
    // std::cout << "list_max_size: " << l1.max_size() << std::endl;


    // std::list<int> myList1;
    // std::list<int> myList2;

    // for (int i = 0; i < 5; ++i)
    // {
    //     myList1.emplace_back(i+1);
    // }

    // for (int i = 0; i < 3; ++i)
    // {
    //     myList1.emplace_back((i+1)*10);
    // }

    // std::vector<int> vec{1, 2, 3};
    // std::deque<int> de{1, 2, 3};

    // stack 的初始化要么是空的,然后通过push压入栈,要么就是把对应的容器转换为 stack (默认的容器是deque)
    // std::deque<int> myDeque{1, 2, 3};
    // std::stack<int> myStack(myDeque);
    // std::cout << "myStack_size: " << myStack.size() << std::endl;

    // std::vector<int> myVector{1, 2};
    // std::stack<int, std::vector<int>> myStack2(myVector);
    // std::cout << "myStack2_size: " << myStack2.size() << std::endl;

    // 和 stack 差不多的方式
    // std::deque<int> myDeque{1, 2, 3};
    // std::queue<int> myQueue(myDeque);
    // std::cout << "myQueue_size: " << myQueue.size() << std::endl;

    // std::vector<int> myVector{1, 2};
    // std::queue<int, std::vector<int>> myQueue2(myVector);
    // std::cout << "myQueue2_size: " << myQueue2.size() << std::endl;

    // priority_queue默认的排序规则是大顶堆,即less比较方法,即首元素是最大值
    // std::priority_queue<int> myPriorityQueue; // 初始化方式1:默认初始化

    // std::vector<int> myVector{1, 2, 3};
    // std::priority_queue<int> myPriorityQueue2(myVector.begin(), myVector.end()); // 初始化方式2:传入两个iterator, 因为默认是vector所以传入数组指针也是可以的

    // std::priority_queue<int> myPriorityQueue3(std::less<int>(), myVector); // 可以通过指定比较函数的方式,直接把容器传进来
    // std::priority_queue<int, std::vector<int>, std::greater<int>> myPriorityQueue4(std::greater<int>(), myVector); // 默认是less,要修改为greater则需要把所有的参数都列出来

    // // 关于pair的比较,默认会先比较元素1, 再比较元素2
    // std::priority_queue<pair<int, int>, std::vector<pair<int, int>>, std::less<pair<int, int>>> myPriorityQueue5;
    // for (int i = 0; i < 5; i++)
    // {
    //     // myPriorityQueue5.push(pair<int, int>(i, i+1));
    //     myPriorityQueue5.emplace(i, i+1); // 使用emplace时只需要传入类型的参数,如果直接传入pair<int, int>(i, i+1)则实际和push没有差别
    // }

    // std::cout << "priority_queue_size = " << myPriorityQueue5.size() << std::endl;
    // std::cout << "priority_queue x = " << myPriorityQueue5.top().first << ", second = " << myPriorityQueue5.top().second << std::endl;
    // // 自定义排序比较
    // std::priority_queue<Node, std::vector<Node>, myCompare> myPriorityQueue6;
    // for (int i = 0; i < 5; i++)
    // {
    //     myPriorityQueue6.emplace(i, i+1);
    // }
    // std::cout << "priority_queue6 x = " << myPriorityQueue6.top().x << ", second = " << myPriorityQueue6.top().y << std::endl;


    // std::vector<Node> myVector;

    // // myVector.push_back(Node(1, 2)); // 效率不高
    // myVector.emplace_back(1, 2); // 效率高

    // std::cout << "success push_back" << std::endl;

    // Node myNode = *myVector.begin();

    // std::cout << "get node" << std::endl;

    // myVector.pop_back();
    // std::cout << "after pop_back" << std::endl;

    // std::vector<std::shared_ptr<Node>> myVector;
    // myVector.push_back(std::make_shared<Node>(1, 2));
    // std::cout << "success push_back" << std::endl;

    // myVector.emplace_back(std::make_shared<Node>(3, 4)); // 在这种情况下 push_back 和 emplace_back 没有区别
    // std::cout << "success emplace_back" << std::endl;

    // auto spNode = *myVector.begin();
    // std::cout << "spNode x = " << spNode->x << ", y = " << spNode->y << std::endl;
    // std::cout << "begin x = " << (*myVector.begin())->x << ", y = " << (*myVector.begin())->y << std::endl;
    
    // 单向链表,
    // std::forward_list<int> myFowrardList;
    // std::forward_list<int> myFowrardList2{1, 2, 3, 4, 5};
    // int val = myFowrardList2.front();
    // std::cout << "val = " << val << std::endl;
    // int val2 = *(++myFowrardList2.begin()); // forward_list 有重载operator++方法,但是不能 .begin() + 1
    // std::cout << "val2 = " << val2 << std::endl;

    // std::set<int> mySet;
    // for (int i = 10; i >= 0; --i)
    // {
    //     mySet.insert(i);
    // }

    // for (auto& m: mySet)
    // {
    //     std::cout << "m = " << m << std::endl;
    // }
    
    // std::map<int, int> myMap;
    // int num = myMap[0];
    // num = 1;
    // std::cout << "maMap[0] = " << myMap[0] << std::endl;

    // int a = 100;
    // int& b = a;
    // b = 200;
    // std::cout << "b = " << b << std::endl;

    // vector<bool> myVec(1);
    // std::cout << "vec = " << *myVec.begin() << std::endl;

    // string s1 = "abcd";
    // string s2 = "abcd";
    // std::cout << (s1 == s2) << std::endl;

    // vector<int> vec1{1, 2, 3, 4, 2, 2, 3, 3, 5}; // 双向迭代器
    // set<int> set1(vec1.begin(), vec1.end());
    // for (auto& num : set1)
    // {
    //     std::cout << num << " ";
    // }
    // std::cout << std::endl;

    // vector<int> vec2(set1.begin(), set1.end());
    
    // auto iter = set1.begin()++;
    // auto iter2 = iter--;
    // for (auto& num: vec2)
    // {
    //     std::cout << num << " ";
    // }
    // std::cout << std::endl;

    // unordered_set<int> set1{1, 2, 3, 4, 5};
    // auto iter = set1.begin(); // 底层是hashtable,是正向迭代器
    // iter++;
    // // iter--; //没有--操作

    // forward_list<int> lst{1, 2, 3, 4};
    // auto iter2 = lst.begin();
    // iter2++;
    // // iter2--; // 正向迭代器,没有--操作
    // string mys = "123";
    // string mys2 = "255";
    // string mys3 = "0";
    // string mys4 = "190";
    
    // std::cout << (mys3 <= mys) << std::endl;

    // string s(5, 'a');
    // std::cout << s << std::endl;
    // for (char i = 'a'; i < 'z'; ++i)
    //     std::cout << i << " " << std::endl;

    // vector<int> vec1{1, 2, 3, 4};
    // vector<int> vec2{5, 6, 7, 8};
    // cout << "vec1 begin: " << &(*vec1.begin()) << endl;
    // cout << "vec2 begin: " << &(*vec2.begin()) << endl;
    // swap(vec1,vec2);
    // cout << "vec1 begin: " << &(*vec1.begin()) << endl;
    // cout << "vec2 begin: " << &(*vec2.begin()) << endl;

    // priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(&myCmp)> myQue(myCmp);
    // priority_queue<pair<int, int>, vector<pair<int, int>>, myCmp2> myQue;
    // myQue.push({1, 2});
    // myQue.push({3, 4});
    // myQue.push({2, 2});
    // myQue.push({1, 1});
    // int n = myQue.size();
    // for (int i = 0; i < n; ++i)
    // {
    //     std::cout << myQue.top().first << " " << myQue.top().second << std::endl;
    //     myQue.pop();
    // }
    // std::cout << std::endl;

    // priority_queue<int, vector<int>, myCmp3> testQue;
    // testQue.push(1);
    // testQue.push(2);
    // testQue.push(2); 
    // testQue.push(3); 
    // testQue.push(4);
    // n = testQue.size();
    // for (int i = 0; i < n; ++i)
    // {
    //     std::cout << testQue.top() << std::endl;
    //     testQue.pop();
    // } 
    // std::cout << std::endl;

    // priority_queue<int> testQue2;
    // testQue2.push(1);
    // testQue2.push(2);
    // testQue2.push(2); 
    // testQue2.push(3); 
    // testQue2.push(4);
    // n = testQue2.size();
    // for (int i = 0; i < n; ++i)
    // {
    //     std::cout << testQue2.top() << std::endl;
    //     testQue2.pop();
    // } 
    // std::cout << std::endl;

    // vector<int> v{2, 4, 1, 3, 9, 0, 2, 5};
    // sort(v.begin(), v.end(), myCmp4());
    // for (auto& n : v) std::cout << n << std::endl;


    // vector<int> myVec{1, 2, 3, 4, 5, 6, 7};
    // vector<int> myVec2{myVec.rbegin(), myVec.rend()}; // 这种方式和 vector<int> myVec2(myVec.rbegin(), myVec.rend());都可以,但是发现
    // // vector<vector<int> myVec3;
    // // myVec3.push_back({myVec.rbegin(), myVec.rend()});//这种方式才可以,但是不能使用()
    // for (auto& num: myVec2)
    //     std::cout << num << " ";
    // std::cout << std::endl;
    // vector<pair<int, int>> prices{{1, 2}, {4, 4}, {3, 1}, {2, 3}, {2, 2}};
    // prices.erase
    // // sort(prices.begin(), prices.end(), greater<pair<int, int>>());
    // // // sort(prices.rbegin(), prices.rend()); // 两种从达到小排序方式
    // // for (auto& kv : prices) cout << kv.first << " " << kv.second << endl;

    // unordered_map<int, vector<int>> myMap;
    // myMap[1].push_back(10);
    // myMap[2].push_back(11);
    // for (auto& kv: myMap) cout << kv.first << " " << kv.second << endl;

    // unordered_map<pair<int, int>, int> myMap; // 因为不接受pair作为key,因此这种方式的定义会报错,需要添加自定义的hash函数
    // unordered_map<pair<int, int>, int, pair_hash> myMap;
    // myMap[{1, 1}] = 1;
    // myMap[{2, 2}] = 2;
    // myMap[{2, 1}] = 3;
    // myMap[{1, 2}] = 4;
    // for (auto& kv: myMap)
    // {
    //     std::cout << kv.first.first << " " << kv.first.second << " " << kv.second << std::endl;
    // }

    // map<pair<int, int>, int> myMap2; // map可以直接把pair当作key,而且他可以接收的是自定义的排序算法,而不是hash算法
    // map<pair<int, int>, int, myCmp2> myMap3;
    // myMap2[{1, 1}] = 1;
    // myMap2[{2, 2}] = 2;
    // myMap2[{2, 1}] = 3;
    // myMap2[{1, 2}] = 4;
    // for (auto& kv: myMap2)
    // {
    //     std::cout << kv.first.first << " " << kv.first.second << " " << kv.second << std::endl;
    // }

    map<int, int> myMap;
    myMap[5] = 1;
    myMap[3] = 2;
    myMap[4] = 4;
    myMap[1] = 5;
    myMap[6] = 6;
    myMap[11] = 7;
    for (auto& kv: myMap) std::cout << kv.first << " " << kv.second << std::endl;
    auto iter = myMap.lower_bound(2);
    std::cout << iter->first << std::endl;
} 