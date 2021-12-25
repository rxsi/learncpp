/*
原位构造可以提升性能
*/

#include<iostream>
#include<vector>
#include<unordered_map>
#include<map>
#include<memory>
using namespace std;

class Test{
public:
    Test(): a(0){
        cout << "Test()" << endl;
    };

    Test(int i): a(i) {
        cout << "Test(i)" << endl;
    }

    ~Test(){
        cout << "~Test()" << endl;
    }

    Test(const Test& rhs){
        cout << "Test(const Test& rhs)" << endl;
        if (this == &rhs) return;
        this->a = rhs.a;
    }

    int val(){
        return a;
    }

private:
    int a;
};

// int main(){
//     vector<Test> res;
//     // for (int i = 0; i < 2; ++i){
//     //     Test t(i);
//     //     res.push_back(t); // 发生了扩容而调用拷贝构造函数,因此会发现拷贝构造函数的调用次数是3次
//     //     // 因此从这里看出 push_back的流程是 构造 -> 拷贝构造 -> 析构(构造出来的对象,拷贝构造的那个要等vector析构时才会析构)

//     // }
//     for (int i = 0; i < 2; ++i){
//         // Test t(i);
//         // res.emplace_back(t); // 如果使用先构造t对象,再传入emplace_back,那么还是会先构造 -> 拷贝构造 -> 析构(构造的对象)
//         res.emplace_back(i); // 这种则会只会有构造,析构要等到vector被析构时.当然如果vector发生了扩容,还是会有拷贝函数的调用
//     }
// }
  

int main(){ 
    unordered_map<int, Test> myMap;
    // myMap[1] = Test(2); // operator[]需要类有无参数的默认构造函数!!!,因为当key不在map中时,会先调用insert传入默认构造函数T()的对象.
    // 比之直接调用insert多了调用默认构造函数,且可以对已有key的val进行修改;而insert不会

    // cout << "operator[] = " << myMap[1].val() << endl;
    // // Test t(3);
    // auto [ite, result] = myMap.emplace(1, 4); // 当key已经在map中时,还是会先调用对应的构造函数创建对象再尔销毁,因此有性能浪费
    // cout << "emplace = " << myMap[1].val() << " result = " << result << endl;
    auto [ite3, result3] = myMap.insert(make_pair(1, 3));
    cout << "insert = " << myMap[1].val() << " result = " << result3 << endl;
    
    // auto [ite4, result4] = myMap.insert(make_pair<int, Test>(1, 4));
    // cout << "insert = " << myMap[1].val() << " result = " << result4 << endl;
    
    // auto [ite5, result5] = myMap.insert(pair<int, Test>(1, 3));
    // cout << "insert = " << myMap[1].val() << " result = " << result5 << endl;
    
    // auto [ite6, result6] = myMap.insert(pair<const int, Test>(1, 6));
    // cout << "insert = " << myMap[1].val() << "result = " << result6 << endl;
    
    // auto [ite7, result7] = myMap.insert(make_pair<const int, Test>(1, 4));
    // cout << "insert = " << myMap[1].val() << "result = " << result7 << endl;
    
    // auto [ite2, result2] = myMap.try_emplace(1, 5); // 当key已经在map中,什么都不做
    // cout << "try_emplace = " << myMap[1].val() << " result = " << result2 << endl;
}

// class Book
// {
//     int m_nId;
// public:
//    // Book() { }  //<----- Why is this required?
//     Book( int id ): m_nId( id ) { }

// };

// int main() {
//     map< int, Book > mapBooks;

//     for( int i = 0; i < 10; ++i )
//     {
//         // Book b( i );
//         mapBooks.insert(make_pair(i, i));
//         // mapBooks[ i ] = b;
//     }
// 	cout  << "test\n";
// 	return 0;
// }