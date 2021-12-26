#ifdef __WIN32__
# include <winsock2.h>
# include <ws2tcpip.h>
#define close(s) closesocket(s)
#else
# include <sys/socket.h>
#endif

#include<iostream>
#include<vector>

int main(int argc, char* argv[]){
    // 创建一个监听socket
    // 参数1: af(地址簇 address family), 常用为 AF_INET(ipv4), AF_INET6(ipv6)
    // 参数2: type(数据传输类型), 常用为 SOCK_STREAM(数据流类型), SOCK_DGRAM(数据包类型)
    // 参数3: protocol(传输协议), 常用为 IPPROTO_TCP(TCP类型), IPPROTO_UDP(UDP类型), 当可以由系统根据上两个参数推算得到,则可直接填0
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1){ // 返回-1则调用失败 
        std::cout << "create listen socket error." << std::endl;
        return -1; // 返回-1表示程序运行错误
    }
    
    // 初始化服务器地址
    sockaddr_in bindaddr;  // C++中struct可以不加
    bindaddr.sin_family = AF_INET;
    #ifdef __WIN32__
        bindaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    #else
        bindaddr.sin_addr.S_addr = htonl(INADDR_ANY);
    #endif
    bindaddr.sin_port = htons(3000);
    if (bind(listenfd, (sockaddr*)&bindaddr, sizeof(bindaddr)) == -1){
        std::cout << "bind listen socket error." << std::endl;
        close(listenfd);
        return -1;
    }

    // 启动监听
    if (listen(listenfd, SOMAXCONN) == -1){
        std::cout << "listen error." << std::endl;
        close(listenfd);
        return -1;
    }

    // 存储客户端socket数组
    std::vector<int> clientfds;
    int maxfd;

    while (true){
        fd_set readset;
        FD_ZERO(&readset);

        // 将监听socket加入待检测的可读事件中
        FD_SET(listenfd, &readset);

        maxfd = listenfd;
        // 将客户端fd加入待检测的可读事件中
        int clientfdslength = clientfds.size();
        for (int i = 0; i < clientfdslength; ++i){
            if (clientfds[i] != -1){
                FD_SET(clientfds[i], &readset);
                if (maxfd < clientfds[i]) maxfd = clientfds[i];
            }
        }

        timeval tm;
        tm.tv_sec = 1;
        tm.tv_usec = 0;
        // 暂且值检测可读事件,不检测可写和异常事件
        int ret = select(maxfd + 1, &readset, NULL, NULL, &tm);
        if (ret == -1){
            // 出错,退出程序
            if (errno != EINTR) break;
        } else if (ret == 0){
            // select函数超时,下次继续
            continue;
        } else{
            // 检测到某个socket有事件
            if (FD_ISSET(listenfd, &readset)){
                // 监听socket的可读事件,表明有新的连接到来
                sockaddr_in clientaddr;
                socklen_t clientaddrlen = sizeof(clientaddr);
                // 接收客户端连接
                int clientfd = accept(listenfd, (sockaddr*)&clientaddr, &clientaddrlen);
                if (clientfd == -1) break; // 接收连接出错,退出程序
                // 只接收连接,不调用recv收取任何数据
                std::cout << "accept a client connection, fd: " << clientfd << std::endl;
                clientfds.emplace_back(clientfd);
            } else{
                // 假设对端发来的数据长度不超过63个字符
                char recvbuf[64];
                int clientfdslength = clientfds.size();
                for (int i = 0; i < clientfdslength; ++i){
                    if (clientfds[i] != -1 && FD_ISSET(clientfds[i], &readset)){
                        memset(recvbuf, 0, sizeof(recvbuf));
                        // 非监听socket, 接收数据
                        int length = recv(clientfds[i], recvbuf, 64, 0);
                        if (length < 0){
                            // 收取数据出错
                            std::cout << "recv data error, clientfd: " << clientfds[i] << std::endl;
                            close(clientfds[i]);
                            // 不直接删除该元素,将该位置的元素标记为-1
                            clientfds[i] = -1;
                            continue;
                        }
                        std::cout << "clientfd: " << clientfds[i] << ", recv data: " << recvbuf << std::endl;
                    }
                }
            }
            
        }

    }

    // 关闭所有客户端socket
    int clientfdslength = clientfds.size();
    for (int i = 0; i < clientfdslength; ++i){
        if (clientfds[i] != -1) close(clientfds[i]);
    }

    // 关闭监听socket
    close(listenfd);
    return 0;
}

/*
结论:
1. 每次调用select都需要把fd_set集合从用户态复制到内核态,且都需要在内核中遍历传递进来的所有fd,开销大
2. 单个进程能够监听的fd数量有限,默认是1024,需要修改宏定义然后重新编译内核才可以修改
3. select函数在每次调用之前都要对传入的参数进行重新设定,效率低
4. 默认的timeval参数每次调用之后都会修改,因此需要每次都重新设定;1)0不阻塞等待,直接返回;2)NULL一直阻塞;3)有其他值则会阻塞等待对应时间
*/