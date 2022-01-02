#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <errno.h>

#define INVALID_FD -1

int main(){
    // 创建一个监听socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == INVALID_FD){
        std::cout << "create listen socket error." << std::endl;
        return INVALID_FD;
    }

    // 将监听socket设置为非阻塞的
    int oldSocketFlag = fcntl(listenfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(listenfd, F_SETFL, newSocketFlag) == INVALID_FD){
        close(listenfd);
        std::cout << "set listenfd to nonblock error." << std::endl;
        return INVALID_FD;
    }

    // 复用地址和端口号
    // SO_REUSEADDR : 用以解决当服务器主动close之后处于TIME_WAIT状态时，重新启动时出现的地址被占用情况
    // SO_REUSEPORT : 用以允许多个socket绑定到相同的ip和port，底层会把接收到的数据负载均衡到绑定的多个socket上
    // 一般用以在多进程/多线程中提高负载。
    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on));
    
    // 初始化服务器的地址
    sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(3000);
    if (bind(listenfd, (sockaddr*)&bindaddr, sizeof(bindaddr)) == INVALID_FD){
        std::cout << "bind listen socket error." << std::endl;
        close(listenfd);
        return INVALID_FD;
    }

    // 启动监听
    if (listen(listenfd, SOMAXCONN) == INVALID_FD){
        std::cout << "listen error." << std::endl;
        close(listenfd);
        return INVALID_FD;
    }

    std::vector<pollfd> fds;
    pollfd listen_fd_info;
    listen_fd_info.fd = listenfd;
    listen_fd_info.events = POLLIN; // 所要监听的事件是读事件
    listen_fd_info.revents = 0;
    fds.push_back(listen_fd_info);

    int n;
    while (true){
        // 参数1：struct pollfd* fds: 指向一个pollfd结构体组成的数组
        // 参数2：nfds_t 参数fds所指向的数组的大小, 本质是 typedef unsigned long int nfds_t;
        // 参数3：poll函数的超时时间，单位毫秒
        // 返回值：< 0：发生异常，需要判断是否是被中断；== 0：设定的监听时间内没有事件发生，继续循环
        // > 0：有事件发生
        /*
        struct pollfd{
            int fd;         待检测事件的fd
            short events;   关心的事件的组合；由程序员设置，用以告诉内核我们所关心的事
            short revents;  检测后得到的事件类型；有内核返回
        }
        events 和 revents的常用参数有：
        POLLIN 数据可读
        POLLOUT 数据可写
        POLLERR 发生异常

        与select相比较，有以下优点：
        1）poll不需要传入最大文件描述符+1的值
        2）与select相比，poll在处理大数量的文件描述符时速度更快
        3）poll没有最大连接数限制，（select的最大连接数由内核的fd_set数据结构限制，默认是1024）
        4）在调用poll函数时，只需要对参数进行因此设置就好
        
        和select一样的缺点：
        1）不管有没有意义，大量的fds都需要在用户态和内核态之间复制拷贝
        2）需要对所有的fds进行遍历，才能得到有事件的fd
        */ 
        n = poll(&fds[0], fds.size(), 1000);
        if (n < 0){
            // 被信号中断
            if (errno == EINTR)
                continue;
            // 出错，退出
            break;
        } else if (n == 0){
            // 超时，继续
            continue;
        }

        // 可见还是要遍历所有的fd，和select本质上无异
        int i = 0;
        while (i < fds.size()){
            if (fds[i].revents & POLLIN){ // 发生的事件是可读事件
                if (fds[i].fd == listenfd){
                    // 监听socket，接受新连接
                    sockaddr_in clientaddr;
                    socklen_t clientaddrlen = sizeof(clientaddr);
                    // 接受客户端连接并将产生的clientfd加入fds集合中
                    int clientfd = accept(listenfd, (sockaddr*)&clientaddr, &clientaddrlen);
                    if (clientfd != INVALID_FD){
                        // 将客户端socket设置为非阻塞
                        int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
                        int newSocketFlag = oldSocketFlag | O_NONBLOCK;
                        if (fcntl(clientfd, F_SETFL, newSocketFlag) == INVALID_FD){
                            close(clientfd);
                            std::cout << "set clientfd to nonblock error." << std::endl;
                        } else {
                            pollfd client_fd_info;
                            client_fd_info.fd = clientfd;
                            client_fd_info.events = POLLIN;
                            client_fd_info.revents = 0;
                            fds.push_back(client_fd_info);
                            std::cout << "new client accepted, clientfd: " << clientfd << std::endl;
                        }
                    }
                    ++i;
                } else {
                    // 普通的clientfd，收取数据
                    // std::vector<char> buf(64); // char buf[64] = {0} 数组初始化形式区别
                    // 使用char数组可以直接调用std::cout 输出
                    char buf[64] = {0};
                    int m = recv(fds[i].fd, buf, 64, 0);
                    if (m <= 0){
                        if (errno != EINTR && errno != EWOULDBLOCK){
                            // 书里面使用iter遍历所有连接，并检测是否等于当前当前fds[i].fd，不知为何
                            // 出错或对端关闭了连接，关闭对应的clientfd，并设置无效标志位
                            close(fds[i].fd);
                            fds.erase(fds.begin()+i);
                        }
                    } else {
                        std::cout << "recv from client: " << buf << ", clientfd: " << fds[i].fd << std::endl;
                        ++i;
                    }
                }
            } else if (fds[i].revents & POLLERR){
                ++i;
                //TODO: 暂不处理
            }
            
        }

        for (auto& clientfd : fds){
            close(clientfd.fd);
        }
        return 0;
    }
}