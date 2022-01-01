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
    listen_fd_info.events = POLLIN;
    listen_fd_info.revents = 0;
    fds.push_back(listen_fd_info);

    // 是否存在无效的fd标志
    bool exist_invalid_fd;
    int n;
    while (true){
        exist_invalid_fd = false;
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

        // @TODO, 查看size_t的意义 
        auto iter = fds.begin();
        while (iter != fds.end()){
            if (iter->revents & POLLIN){
                if (iter->fd == listenfd){
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
                    ++iter;
                } else {
                    // 普通的clientfd，收取数据
                    std::vector<char> buf(64); // char buf[64] = {0} 数组初始化形式区别
                    int m = recv(iter->fd, buf, 64, 0);
                    if (m <= 0){
                        if (errno != EINTR && errno != EOULDBLOCK){
                            // 书里面使用iter遍历所有连接，并检测是否等于当前当前fds[i].fd，不知为何
                            // 出错或对端关闭了连接，关闭对应的clientfd，并设置无效标志位
                            close(iter->fd);
                            iter = fds.erase(iter);
                            // iter->fd = INVALID_FD;
                            // exist_invalid_fd = true;
                        }
                    } else {
                        std::cout << "recv from client: " << buf << ", clientfd: " << iter->fd << std::endl;
                        ++iter;
                    }
                }
            } else if (iter->revents & POLLERR){
                ++iter;
                //TODO: 暂不处理
            }
            
        }
        // for (size_t i = 0; i < fds.size(); i++){
        //     // 事件可读
        //     if (fds[i].revents & POLLIN){
        //         if (fds[i].fd == listenfd){
        //             // 监听socket，接受新连接
        //             sockaddr_in clientaddr;
        //             socklen_t clientaddrlen = sizeof(clientaddr);
        //             // 接受客户端连接并将产生的clientfd加入fds集合中
        //             int clientfd = accept(listenfd, (sockaddr*)&clientaddr, &clientaddrlen);
        //             if (clientfd != INVALID_FD){
        //                 // 将客户端socket设置为非阻塞
        //                 int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
        //                 int newSocketFlag = oldSocketFlag | O_NONBLOCK;
        //                 if (fcntl(clientfd, F_SETFL, newSocketFlag) == INVALID_FD){
        //                     close(clientfd);
        //                     std::cout << "set clientfd to nonblock error." << std::endl;
        //                 } else {
        //                     pollfd client_fd_info;
        //                     client_fd_info.fd = clientfd;
        //                     client_fd_info.events = POLLIN;
        //                     client_fd_info.revents = 0;
        //                     fds.push_back(client_fd_info);
        //                     std::cout << "new client accepted, clientfd: " << clientfd << std::endl;
        //                 }
 
        //             }
        //         } else {
        //             // 普通的clientfd，收取数据
        //             std::vector<char> buf(64); // char buf[64] = {0} 数组初始化形式区别
        //             int m = recv(fds[i].fd, buf, 64, 0);
        //             if (m <= 0){
        //                 if (errno != EINTR && errno != EOULDBLOCK){
        //                     // 书里面使用iter遍历所有连接，并检测是否等于当前当前fds[i].fd，不知为何
        //                     // 出错或对端关闭了连接，关闭对应的clientfd，并设置无效标志位
        //                     close(fds[i].fd);
        //                     fds[i].fd = INVALID_FD;
        //                     exist_invalid_fd = true;
        //                 }
        //             } else {
        //                 std::cout << "recv from client: " << buf << ", clientfd: " << fds[i].fd << std::endl;
        //             }
        //         }
        //     } else if (fds[i].revents & POLLERR){
        //         //TODO: 暂不处理
        //     }

        // }

        for (auto& clientfd : fds){
            close(clientfd.fd);
        }
        return 0;
    }
}