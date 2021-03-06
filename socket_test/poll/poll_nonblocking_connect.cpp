#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#define SERVER_ADDRESS  "127.0.0.1"
#define SERVER_PORT     3000
#define SEND_DATA       "helloworld"

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
    // 这里采用poll模式进行判断.
    // 如果返回值<1,
    // 在linux平台,不管是否可写,这里都会直接返回 1, 即可写,因此还需要额外判断
    pollfd event;
    event.fd = clientfd;
    event.events = POLLOUT;
    int timeout = 3000;
    if (poll(&event, 1, timeout) != 1){
        close(clientfd);
        std::cout << "[poll] connect to server error." << std::endl;
        return -1;
    } 

    if (!(event.revents & POLLOUT)){
        close(clientfd);
        std::cout << "[POLLOUT] connect to server error." << std::endl;
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