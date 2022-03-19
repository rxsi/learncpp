#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
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

    // 创建一个epollfd，使用epoll_create(int size)
    // 这个size值只需要填入大于0的值即可
    int epollfd = epoll_create(1);
    if (epollfd == -1){
        std::cout << "create epollfd error." << std::endl;
        close(listenfd);
        return INVALID_FD;
    }

    epoll_event listen_fd_event;
    listen_fd_event.data.fd = listenfd;
    listen_fd_event.events = EPOLLIN;
    // 若取消注释这一行，则使用ET模式
    // listen_fd_event.events |= EPOLLET;
    // 将监听socket绑定到epollfd上
    // 参数1: int epfd : 通过epoll_create生成的epollfd
    // 参数2: int op : 操作类型,可选值有:1) EPOLL_CTL_ADD, EPOLL_CTL_MOD, EPOLL_CTL_DEL
    // 参数3: int fd : 要操作的fd
    // 参数4: struct epoll_event * event : 
    /* struct epoll_event{
        uint32_t events; // 需要检测的fd事件标志,可选有: EPOLLIN, EPOLLERR
        epoll_data_t data; // 用户自定义数据,本质是一个union对象,在64位操作系统中大小是8字节
    }

    typedef union epoll_data{
        void* ptr;
        int fd;
        uint32_t u32;
        uint64_t u64;
    } epoll_data_t;
    */
    // 返回值: == 0 : 成功调用,-1调用失败
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &listen_fd_event) == INVALID_FD){
        std::cout << "epoll_ctl error." << std::endl;
        close(listenfd);
        return INVALID_FD;
    }

    int n;
    while (true){
        epoll_event epoll_events[1024];
        // 参数1: int epfd : 由epoll_create的epollfd
        // 参数2: epoll_event结构体数组的首地址,当然如果只需要监听一个,直接传入一个epoll_event结构体的地址即可
        // 参数3: int maxevents : 参数2的数量
        // 参数4: int timeout : 超时时间,单位毫秒

        // 返回值: 0 : 超时, -1 : 失败, > 0 : 有事件的fd数量
        n = epoll_wait(epollfd, epoll_events, 1024, 1000);
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

        for (size_t i = 0; i < n; ++i){
           if (epoll_events[i].events & EPOLLIN){ // 发生的事件是可读事件
                if (epoll_events[i].data.fd == listenfd){
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
                            epoll_event client_fd_event;
                            client_fd_event.data.fd = clientfd;
                            // 同时监听新建立的连接socket的读和写事件
                            client_fd_event.events = EPOLLIN | EPOLLOUT;
                            // 若取消注释这一行，则使用ET模式
                            client_fd_event.events |= EPOLLET;
                            // 默认是LT模式,即有数据可读时总是返回EPOLLIN
                            // 使用ET模式,则只有当有新数据输入时才会有事件,因此需要我们使用循环一直读取数据直到触发返回-1,且错误码为EWOULDBLOCK
                            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &client_fd_event) != INVALID_FD){
                                std::cout << "new client accepted, clientfd: " << clientfd << std::endl;
                            } else {
                                std::cout << "add client fd to epollfd error." << std::endl;
                                close(clientfd);
                            }
                        }
                    }
                } else {
                    // 普通的clientfd，收取数据
                    std::cout << "client fd: " << epoll_events[i].data.fd << " recvdata." << std::endl;

                    char recvbuf[1024] = {0};
                    int m = recv(epoll_events[i].data.fd, recvbuf, 1024, 0);
                    if (m <= 0){
                        if (errno != EINTR && errno != EWOULDBLOCK){
                            if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1){
                                std::cout << "client disconnected, clientfd: " << epoll_events[i].data.fd << std::endl;
                            }
                            close(epoll_events[i].data.fd);
                        }
                    } else {
                        std::cout << "recv from client: " << recvbuf << ", clientfd: " << epoll_events[i].data.fd << std::endl;

                        /*/果把这里注释掉,那么只会触发一次可写,也就是说如果使用ET模式,那么每次检查过后,都需要重新注册模式*/
                        epoll_event client_fd_event;
                        client_fd_event.data.fd = epoll_events[i].data.fd;
                        // 再次给clientfd注册检测可写事件
                        client_fd_event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                        if (epoll_ctl(epollfd, EPOLL_CTL_MOD, epoll_events[i].data.fd, &client_fd_event) != -1){
                            std::cout << "epoll_ctl successfully, mode: EPOLL_CTL_MOD, clientfd:" << epoll_events[i].data.fd << std::endl;
                        }
                    }
                }
            } else if (epoll_events[i].events & EPOLLOUT){
                if (epoll_events[i].data.fd != listenfd){
                    std::cout << "EPOLLOUT triggered, clientfd: " << epoll_events[i].data.fd << std::endl; // 采用lt模式时,会一直触发
                }
            } else if (epoll_events[i].events & EPOLLERR){
                //TODO: 暂不处理
            }
        }
    }
    close(listenfd);
    return 0;
}