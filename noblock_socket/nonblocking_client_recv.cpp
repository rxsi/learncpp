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

    int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(clientfd, F_SETFL, newSocketFlag) == -1){
        close(clientfd);
        std::cout << "set socket to nonblock error." << std::endl;
        return -1;
    }

    while (true){
        std::vector<char> recvbuf(32, 0); //使用vector<char>作为缓冲区,更好控制.
        // 返回值: >0 表示接收到了数据
        // -1 && (errno == EWOULDBLOCK || errno == EAGAIN),没有数据接收,对端没有发送数据过来
        // -1 && errno == EINTR , 被打断,继续执行即可.
        // 0 对端关闭了socket
        int ret = recv(clientfd, &recvbuf, recvbuf.size(), 0); 
        if (ret > 0){
            std::cout << "recv successfully." << std::endl;
        } else if (ret == 0){ // 这里当收到0时表示了关闭了socket,那么如果对端发送0字节呢???
            // 答案是对端发送的0字节数据不会被发出来,而是直接过滤,也就是说send返回值为0有两种2情况:
            // 1) 对端关闭了,而尝试使用send发送数据
            // 2) 尝试发送0字节 (并不会实际发出,而被直接过滤了)

            // recv端返回0的情况:
            // 1) 对端连接关闭
            std::cout << "peer close the socket." << std::endl;
            break;
        } else if (ret == -1){
            if (errno == EWOULDBLOCK){
                std::cout << "There is no data avaliable now." << std::endl;
            } else if (errno == EINTR){
                std::cout << "recv datainterrupted by signal." << std::endl;
            } else{
                break;
            }
        }
    }

    close(clientfd);
    return 0;
}