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
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS); // 解析服务端ip地址
    serveraddr.sin_port = htons(SERVER_PORT);
    if (connect(clientfd, (sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
        std::cout << "connect socket error." << std::endl;
        close(clientfd);
        return -1;
    }

    std::vector<char> recvbuf(32, 0); //使用vector<char>作为缓冲区,更好控制.
    // 参数1: int fd: 可接收数据的fd
    // 参数2: buf: char类型数组,或者vector<char>
    // 参数3: 控制参数,一般为0
    int ret = recv(clientfd, &recvbuf, recvbuf.size(), 0); // 当没有接收到数据时,会阻塞在这里

    if (ret > 0){
        std::cout << "recv successfully." << std::endl;
    } else{
        std::cout << "recv data error." << std::endl;
    }

    close(clientfd);
    return 0;
}