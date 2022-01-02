/*
vector测试
*/
#include<iostream>
#include<vector>
using namespace std;

/*
vector底层是连续的内存段

*/
int main(){
    vector<int> vec = {1, 2, 3, 4, 5};
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
    int iter = 0; 
    while (iter < vec.size()){
        if (vec[iter] <= 3 && vec[iter] >= 1){
            vec.push_back(vec[iter]+10);
            ++iter;
        } else if (vec[iter] >= 4 && vec[iter] <= 5){
            vec.erase(vec.begin()+iter);
        } else {
            ++iter;
        }
    }

    for (auto& val : vec){
        std::cout << val << " ";
    }
    std::cout << std::endl;

}