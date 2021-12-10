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
    vector<int> vec;
    for (int i = 0; i < 10; ++i){
        vec.push_back(i);
    }
    vec.reserve(3);
    cout << vec.size() << endl;
    cout << vec.capacity() << endl;
    cout << vec[0] << endl;
    cout << vec.max_size() << endl;
}