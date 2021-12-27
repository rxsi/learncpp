#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<string.h>
#include<fcntl.h>
#include<stdio.h>
#include<errno.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 3000
#define SEND_DATA "hello_world"

int main(){
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1){
        std::cout << "create client socket error. " << std::endl;
        return -1;
    }
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS); // 解析服务端ip地址
    serveraddr.sin_port = htons(SERVER_PORT);
    if (connect(clientfd, (sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
        std::cout << "connect socket error." << std::endl;
        close(clientfd);
        return -1;
    }

    // 设置send函数为非阻塞模式
    // 如果在connect之前就设置,那么会把connect也变成非阻塞
    int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(clientfd, F_SETFL, newSocketFlag) == -1){
        close(clientfd);
        std::cout << "set socket to nonblock error." << std::endl;
        return -1;
    }

    int count = 0;
    while (true){
        // 参数1: int fd: 可发送数据的fd套接字
        // 参数2: 要发送的数据的缓冲区
        // 参数3: 指明buff的长度
        // 参数4: int flag: 定义一些设置,一般为0

        // 返回值: 如果返回值不等于buff长度,则发送数据失败
        // 这里是非阻塞模式: 返回-1 且错误码等于 EWOULDBLOCK: 对端的TCP接收窗口太小,暂时无法发送出去
        // 错误码等于EINTR,被信号中断,可以直接重试
        // 其他错误码出错
        int ret = send(clientfd, SEND_DATA, strlen(SEND_DATA), 0);
        if (ret == -1){
            if (errno == EWOULDBLOCK){
                std::cout << "send data error as TCP Window size is too small." << std::endl;
                continue;
            } else if (errno == EINTR){
                std::cout << "sending data interrupted by signal." << std::endl;
                continue;
            } else{
                std::cout << "send data error." << std::endl;
                break;
            }
        } else if (ret == 0){
            std::cout << "send data error." << std::endl;
            close(clientfd);
            break;
        } else if(ret == strlen(SEND_DATA)){ // 这种方法不推荐,应该采用记录偏移量而循环发送,直至发送的所有数据长度等于strlen(SEND_DATA)
            count++;
            std::cout << "send data successfully, count = " << count << std::endl;
        }
    }
    close(clientfd);
    return 0;
}

/*
// 默认的socket是阻塞模式的,设置为非阻塞模式的方法有:

// 1. 这种方式可以在任意阶段修改为非阻塞的
int sockfd;
int oldSocketFlag = fcntl(sockfd, F_GETFL, 0);
int newSocketFlag = oldSocketFlag | O_NONBLOCK;
fcntl(sockfd, F_SETFL, newSocketFlag);

// 2. 创建时就设置为非阻塞
int s = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

// 3. 将accpet产生的fd设置为非阻塞
accept4(listenfd, &clientaddr, &addrlen, SOCK_NONBLOCK);
*/

// 查看某个端口的占用: lsof -i -Pn | grep 端口号