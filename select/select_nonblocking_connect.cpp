#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<string.h>
#include<fcntl.h>
#include<stdio.h>
#include<errno.h>
#include<vector>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 3000
#define SEND_DATA "hello_world"

int main(){
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1){
        std::cout << "create client socket error. " << std::endl;
        return -1;
    }

    int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(clientfd, F_SETFL, newSocketFlag) == -1){
        close(clientfd);
        std::cout << "set socket to nonblock error." << std::endl;
        return -1;
    }

    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS); // 解析服务端ip地址
    serveraddr.sin_port = htons(SERVER_PORT);

    while (true){
        // 非阻塞的connect函数会立即返回
        // 返回0, 表示连接成功,正常的逻辑就应该break跳出循环,这里测试用,因此直接close,然后再return
        // 返回-1 && errno == EINTR: 被中断了,继续循环
        // 返回 -1 && errno == EINPROGRESS: 连接正在重试
        // 其他情况为异常.
        int ret = connect(clientfd, (sockaddr*)&serveraddr, sizeof(serveraddr));
        if (ret == 0){
            std::cout << "connect to server successfully." << std::endl;
            close(clientfd);
            return 0;
        } else if (ret == -1){
            if (errno == EINTR){
                std::cout << "connecting interruptted by signal, try again." << std::endl;
                continue;
            } else if (errno == EINPROGRESS){ // 连接正在重试,不用循环继续调用,直接break,属于正常情况
                std::cout << "auto try connect now, don't need to try again." << std::endl;
                break;
            } else{
                close(clientfd);
                std::cout << "connect error." << std::endl;
                return -1;
            }
        }
    }

    // 根据上面的分析,有可能是处理"连接正在重试" 阶段,因此需要判断是否连接成功.
    // 这里采用select模式进行判断.
    // 在linux平台,不管是否可写,这里都会直接返回 1, 即可写,因此还需要额外判断
    // select监听的返回值类型:
    // 0: 超时
    // 1: 

    fd_set writeset;
    FD_ZERO(&writeset);
    FD_SET(clientfd, &writeset);
    timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    // 注意,这里监听的是写事件,当可写入时会立即返回.因此表现出来是不阻塞的样子
    // 和读事件一般会有较长的阻塞不一样..
    if (select(clientfd+1, NULL, &writeset, NULL, &tv) != 1){ // 正常情况下不管是否连接成功,都会返回1,因此当超时之后返回的是非1,则代表异常,应该直接关闭. 
        std::cout << "[select] connect to server error." << std::endl;
        close(clientfd);
        return -1;
    }

    // 补充判断阶段:
    int err;
    socklen_t len = static_cast<socklen_t>(sizeof(err));
    if(::getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &err, &len) < 0){
        close(clientfd);
        return -1;
    }

    if (err == 0){
        std::cout << "connect to server successfully." << std::endl;
    } else{
        std::cout << "connect to server error." << std::endl;
    }

    close(clientfd);
    return 0;
}

// 模拟服务端: nc -v -l  3000

// 设置连接时顺便接收第一组数据
// 使用setsockopt 设置参数 TCP_DEFER_ACCEPT
// 通过该参数:
// 服务端: bind -> listen -> 客户端:connect -> send -> 服务端:accept -> recv
// 如果没有该参数:
// 服务端: bind -> listen -> 客户端:connect -> 服务端:accept -> 客户端:send -> 服务端:recv



/*
获取当前接收缓冲区的大小
使用 ioctl
示例:
ulong bytesToRecv = 0; // 注意linux平台每次都要置零
if (ioctl(fd, FIONREAD, &bytesToRecv) == 0){
    cout << "get byteslen: " << bytesToRecv << endl;
}

需要注意,不要使用该参数去设置用户态接收缓冲区的大小,错误做法:
ulong bytesToRecv = 0; 
if (ioctl(fd, FIONREAD, &bytesToRecv) != 0) return; //出错

vector<char> RecvBuf(bytesToRecv);
int ret = recv(fd, &RecvBuf, bytesToRecv, 0);

因为很有可能在创建vector缓冲区时,内核态的接收缓冲区就又增长了,因此接收的数据不完整.
应该根据业务需求限定固定的接收字节数
*/
