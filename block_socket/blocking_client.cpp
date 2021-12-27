#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<string.h>

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
    int count = 0;
    while (true){
        // 参数1: int fd: 可发送数据的fd套接字
        // 参数2: 要发送的数据的缓冲区
        // 参数3: 指明buff的长度
        // 参数4: int flag: 定义一些设置,一般为0

        // 返回值: -1 发送失败,如果是在阻塞模式,那么一定是失败了,应该退出.非阻塞模式则另说,详看nonblocking_client.cpp
        // > 0 而不等于strlen(SEND_DATA) 那么表示只发送成功了一部分的数据,应该采用在一个循环里面根据偏移量发送数据
        // 等于strlen(SEND_DATA) 完整的发送数据成功
        int ret = send(clientfd, SEND_DATA, strlen(SEND_DATA), 0); // 这里使用的是阻塞模式,当无法发送时,会阻塞在这里.
        if (ret != strlen(SEND_DATA)){
            std::cout << "send data error." << std::endl;
            break;
        } else{
            count++;
            std::cout << "send data successfully, count = " << count << std::endl;
        }
    }
    close(clientfd);
    return 0;
}