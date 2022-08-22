/*
测试单次最高申请多少内存
测试多次累计最高申请多少内存
*/
#include <iostream>
#include <string.h>

int main(int argc, char *argv[])
{
    size_t nbytes; // 每次申请的内存数
    int times; // 申请次数
    if (argc < 2 || argc > 3)
    {
        std::cout << "err arguments" << std::endl;
        return 1;
    }
    if (argc == 2)
    {
        times = 1;
        nbytes = (size_t)atoi(argv[1]) * 1024 * 1024 * 1024; // 单位G
    }
    if (argc == 3)
    {
        times = atoi(argv[1]);
        nbytes = (size_t)atoi(argv[2]) * 1024 * 1024 * 1024; // 单位G
    }
    char *addr[times];
    for (int i = 0; i < times; ++i)
    {
        addr[i] = (char*) malloc(nbytes);
        if (!addr[i])
        {
            std::cout << "malloc " << nbytes << " err : " << strerror(errno) << std::endl;
            return -1;
        }
        else
        {
            std::cout << "malloc success : " << nbytes << std::endl;
        }
    }
    getchar(); // 让程序卡住
    return 0;
}