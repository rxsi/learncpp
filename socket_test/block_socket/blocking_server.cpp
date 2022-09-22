#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>
// #include<sys/types.h>
#include<iostream>

int main(){
    int listenfd = socket(AF_INET, SOCK_STREAM, 0); // #include<sys/socket.h>
    if (listenfd == -1){
        std::cout << "create listen sokcet error." << std::endl;
        return -1;
    }

    sockaddr_in bindaddr; // #include<arpa/inet.h>
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(3000);
    if (bind(listenfd, (sockaddr*)&bindaddr, sizeof(bindaddr)) == -1){
        std::cout << "bind listen socket error." << std::endl;
        close(listenfd); // #include<unistd.h>
        return -1;
    }
    if (listen(listenfd, SOMAXCONN) == -1){
        std::cout << "listen error." << std::endl;
        close(listenfd);
        return -1;
    }

    // while (true){
    //     sockaddr_in clientaddr;
    //     socklen_t clientaddrlen = sizeof(clientaddr);
    //     int clientfd = accept(listenfd, (sockaddr*)&clientaddr, &clientaddrlen);
    //     if (clientfd != -1){ // 这里不调用recv进行数据接收,探究tcp滑动窗口,接收缓冲区,发送缓冲区的一些问题
    //         std::cout << "accept a client connection. " << std::endl;
    //     }
    // }

    sockaddr_in clientaddr;
    socklen_t clientaddrlen = sizeof(clientaddr);
    int clientfd = accept(listenfd, (sockaddr*)&clientaddr, &clientaddrlen);
    if (clientfd != -1){ // 这里不调用recv进行数据接收,探究tcp滑动窗口,接收缓冲区,发送缓冲区的一些问题
        std::cout << "accept a client connection. " << std::endl;
    }
    close(clientfd);
    close(listenfd);
    return -1;
}
/*

tcpdump -i any -nn -S 'tcp port 3000'

使用tcpdump进行抓包分析,可以看到链接时首先进行三次握手:
14:13:30.790838 IP 127.0.0.1.34154 > 127.0.0.1.3000: Flags [S], seq 876545698, win 43690, options [mss 65495,sackOK,TS val 4180502 ecr 0,nop,wscale 7], length 0
14:13:30.790850 IP 127.0.0.1.3000 > 127.0.0.1.34154: Flags [S.], seq 3266404484, ack 876545699, win 43690, options [mss 65495,sackOK,TS val 4180502 ecr 4180502,nop,wscale 7], length 0
14:13:30.790859 IP 127.0.0.1.34154 > 127.0.0.1.3000: Flags [.], ack 3266404485, win 342, options [nop,nop,TS val 4180502 ecr 4180502], length 0

而后从server -> client的数据中的win参数先增大而后减小.这是因为流量控制和拥塞控制机制的存在,最后win为0,表示server端的接收缓冲区已经满了.

send 是把用户态的发送缓冲区数据copy到内核态的发送缓冲区,而何时发送由系统自行决定.这会造成粘包现象,可以禁用nagle算法,使系统有一个包就立即发送一个包
recv 是把内核态的接收缓冲区数据copy到用户态的接收缓冲区,如果长时间不接收,如本例,则会导致win减小,最后使得client停止发送

*/