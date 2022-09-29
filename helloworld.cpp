// #include <algorithm>
// #include <iostream>
// #include <memory>
// #include <string>
// #include <tuple>
// #include <vector>
// #include <stdio.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/stat.h>

// using namespace std;



// // class StrVec{
// //     public:
// //         StrVec():elements(nullptr), first_free(nullptr), cap(nullptr){} // 默认构造函数
// //         StrVec(const StrVec&); // 拷贝初始化函数，传入的是目标复制对象的引用
// //         StrVec &operator=(const StrVec&); // 拷贝赋值运算符，传入的是右侧对象的引用（左侧对象绑定隐式this），因为调用场景是a = b，因此返回一个指向左侧运算对象的引用（*this）
// //         ~StrVec(); // 析构函数，不带参数

// //         void push_back(const string&); // 拷贝元素
// //         template<typename... Args> void emplace_back(Args&&...); // 移动元素

// //         size_t size() const{
// //             return first_free - elements;
// //         }
        
// //         size_t capacity() const{
// //             return cap - elements;
// //         }

// //         size_t free_size() const{
// //             return cap - first_free;
// //         }

// //         string *begin() const{
// //             return elements;
// //         }

// //         string *end() const{
// //             return first_free;
// //         } 


// //     private:
// //         static allocator<string> alloc; // 分配元素

// //         void chk_n_alloc(){
// //             if(free_size() <= 0){
// //                 reallocate();
// //             }
// //         }
// //         pair<string*, string*> alloc_n_copy(const string*, const string*);
// //         void free();
// //         void reallocate();
// //         string *elements; // 头指针，固定不变，除非重申请内存
// //         string *first_free; // 最后的元素的尾指针
// //         string *cap; // 总内存的最后的指针，固定不变，除非重申请内存
// // };

// // void StrVec::push_back(const string &s){
// //     chk_n_alloc(); // 检查空间是否足够
// //     alloc.construct(first_free++, s); // 将s存入first_free指针所指位置，然后递增first_free指针, construct负责在指定指针位置初始化对象
// // }

// // template<typename... Args> inline void StrVec::emplace_back(Args&&... args) // typename... Args： 表示是可变参数（0个或多个，且类型可以不一样）Args是模板参数包， Args&&... args:表示可变参数，类型为Args右值引用，args是函数参数包
// // {
// //     chk_n_alloc(); // 先确保有足够的空间
// //     alloc.construct(first_free++, std::forward<Args>(args)...); // std::forward<Args>转发, ...是包扩展（列表解析），代表着对args中的每个参数都应用std::forward<Args>d的
// // }

// // pair<string*, string*> StrVec::alloc_n_copy(const string *b, const string *e){
// //     auto data = alloc.allocate(e - b); // 分配指定大小（n * StrVec）的未初始化的内存空间，e尾指针，b头指针， n = e-b 中间的元素数量,data是新内存的头指针
// //     return {data, uninitialized_copy(b, e, data)}; // uninitialized_copy(头指针，尾指针，未初始化的内存的起始地址)返回存入数据后的尾指针，最后return的结果是新分配内存的头指针和尾指针
// // }


// // void StrVec::free(){
// //     if (elements){ // 当elements不是空指针即证明有分配内存，因此需要遍历删除旧数据
// //         for (auto p = first_free; p != elements;){
// //             alloc.destroy(--p); // destory 会调用分配元素的析构函数，本例中是string
// //         }
// //         alloc.deallocate(elements, cap - elements); // 释放申请的空间
// //     }
// // }

// // StrVec::StrVec(const StrVec &s){
// //     auto newdata = alloc_n_copy(s.begin(), s.end()); // 返回新内存的头指针和尾指针
// //     elements = newdata.first;
// //     first_free = cap = newdata.second;
// // }

// // StrVec::~StrVec(){
// //     free(); // 析构时调用free，释放资源
// // }

// // // 拷贝赋值运算符
// // StrVec &StrVec::operator=(const StrVec &rhs){
// //     auto data = alloc_n_copy(rhs.begin(), rhs.end()); // 根据所要拷贝的数据的头指针和尾指针，重新申请新的内存，返回的是pair
// //     free(); // 释放旧空间
// //     elements = data.first; // 指定为头指针
// //     first_free = cap = data.second; // 指定尾指针
// //     return *this;
// // }

// // void StrVec::reallocate(){
// //     auto newcapacity = size() ? 2 * size() : 1; // 计算新的空间大小，按照2倍增长率
// //     auto newdata = alloc.allocate(newcapacity); // 根据新的空间大小申请未初始化的空间,返回新的内存空间的头指针
// //     auto desc = newdata; 
// //     auto elem = elements;
// //     for (size_t i = 0; i != size(); ++i){
// //         alloc.construct(desc++, std::move(*elem++)); // 使用move指令对旧的数据进行移动，避免使用拷贝
// //     }
// //     free(); // 释放旧的空间
// //     elements = newdata;
// //     first_free = desc;
// //     cap = elements + newcapacity;
// // }

// #define BUFFER_SIZE 1024
// #define FILE_PATH "/home/rxsi/text.txt"

// void printFileMessage(const char* s)
// {
//     struct stat st;
//     stat(FILE_PATH, &st);
//     printf("%s: file_size: %d, file_link: %d, blk_num: %d\n", s, st.st_size, st.st_nlink, st.st_blocks);
// }


// int main(){
//     //vector<int> s = {0, 2, 3, 4, 5};
//     //cout << s.end() - s.begin() << endl;
//     //int a = 1;
//     //int b = 2;
//     //int c = a;
//     //std::cout << "a = " << a << "b = " << b << std::endl;
//     //cout << "&a = " << &a << "&b = " << &b << endl;
//     //using std::swap;
//     //swap(a, b);
//     //std::cout << "a = " << a << "b = " << b << std::endl;
// //    int a = 1;
// //    int &b = a;
// //    int *c = &a;
// //    int &&d = 1;
// //    cout << &a << " " << &b << " " << &c << " " << &d << endl;

// //    string s = "123"; // const char* 可以直接转换为string类对象，是因为string的构造方法的参数可以是const char*
// //    const char *s1 = "456";
// //    // char *s2 = "789"; "789"是const char* ,不能把一个const转换成非const
// //    cout << s << endl;    
// //    cout << s1 << endl;
// //
// //    const char *s3 = s.c_str(); // string类对象使用c_str()方法转换为const char*
// //    cout << s3 << endl;
// //
//     // 如果一个赋值运算符被设计成了成员函数，则左侧运算对象必须是运算符所属类的一个对象
//     // 成员运算符的等价调用：
//     // data1 + data2; => data1.operator+(data2); this隐式的指向了data1
//     // 非成员运算符的等价调用：
//     // data1 + data2; => operator+(data1, data2);
//     // string s = "world";
//     // string t = s + "!";
//     // string u = "hi" + s;// 因为string的operator+是非成员函数，因此该调用是正确的.
//     // cout << s << endl;
//     // cout << t << endl;
//     // cout << u << endl;
//     int fd;
//     char buffer[BUFFER_SIZE];

//     fd = open(FILE_PATH, O_RDWR);
//     if (fd == -1)
//     {
//         printf("open file error \n");
//         exit(1);
//     }

//     int pid = fork();
//     if (pid == 0)
//     {
//         int i = 0;
//         while (1)
//         {
//             i++;
//             int n = read(fd, buffer, BUFFER_SIZE-1);
//             printFileMessage("read process");
//             printf("file_offset: %d\n", lseek(fd, 0, SEEK_CUR));
//             if (n == -1)
//             {
//                 printf("read error \n");
//                 exit(1);
//             }
//             buffer[n] = '\n';
//             printf("%d pid:%d, fd:%d, content: %s \n", i, getpid(), fd, buffer);
//             cout << endl;
//             sleep(1.5);
//         }
//     }
//     else if (pid > 0)
//     {
//         int i = 0;
//         while (1)
//         {
//             i++;
//             char s[BUFFER_SIZE] = "write message!!!";
//             int n = write(fd, s, BUFFER_SIZE);
//             printFileMessage("write process");
//             printf("file_offset: %d \n", lseek(fd, 0, SEEK_CUR));
//             if (n == -1)
//             {
//                 printf("write error \n");
//                 exit(1);
//             }
//             printf("%d pid:%d, fd:%d \n", i, getpid(), fd);
//             sleep(1);
//         }
//     }
    
//     close(fd);
//     return 0;
// }

#include <random>
#include <iostream>
#include <string.h>

int main()
{
    while (true)
    {
        // int j = rand() % 10;
        // int b = 1 / j;
        // std::cout << b << std::endl;
        char* p = (char*)malloc(10000000);
        memset(p, '\0', 10000000);
    }
    return 0;
}