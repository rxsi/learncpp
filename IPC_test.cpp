/*
进程通信方式
*/

/*
1. 匿名管道
管道是通过在内核区开辟一块缓冲区,进程A把数据从用户空间拷贝到内核缓冲区,进程B再从内核缓冲区把数据复制到用户空间.底层的数据结构是环形队列
在虚拟内存中,以32位系统为例,高地址的1G空间为内核空间,所有进程共享
主要用于在父子进程/兄弟进程间通信
管道是单通道的

流程:
1. 在当前进程(父进程)创建管道,
2. 再通过fork的方式创建子进程,子进程会继承复制管道的文件描述符(task_struct结构体中的files_struct)
3. 父子进程分别关闭一端,即可实现父子进程间的数据通信,使用write和read函数

创建
#include <unistd.h>
int pipe(int fd[2]); 成功返回0, 出错返回-1
fd[0]指向管道的读端,fd[1]指向管道写端

管道数据读取的四种情况:
1. 读端不读,写端一直写,读端不关闭fd[0]:
    当内核缓冲区被写满之后,会阻塞写端的write函数,缓冲区大小为65535(64K大小)

2. 写端不写,读端一直读,写端不关闭fd[1]:
    当内核缓冲区数据被读完之后,会阻塞读端的read函数,缓冲区大小为65535(64K大小)

3. 读端不读,写端一直写,读端关闭fd[0]:
    写端调用write时会收到SIGPIPE信号,这个信号一般会导致进程异常终止

4. 写端不写,读端一直读,写端关闭fd[1]:
    当读端read完缓冲区数据再次read时会返回0
*/

// #include <unistd.h>
// #include <errno.h>
// #include <stdio.h>
// #include <string.h> // .h是C语言的头文件,没有.h是C++的头文件

// int main()
// {
//     int fd[2];
//     int ret = pipe(fd);
//     if (ret == -1)
//     {
//         perror("pipe error\n"); // 将保存输出到标准错误输出stderr中.0是标准输入,1是标准输出,3是标准错误输出
//         return 1;
//     }
//     pid_t id = fork();
//     if (id == 0) // child
//     {
//         int i = 0;
//         close(fd[0]); // 关闭读端
//         const char* child = "I am child";
//         while (i < 5)
//         {
//             write(fd[1], child, strlen(child)+1);
//             sleep(2);
//             i++;
//         }
//     }
//     else if (id > 0) // father
//     {
//         close(fd[1]); // 关闭写端
//         char msg[100];
//         int j = 0;
//         while (j < 5)
//         {
//             memset(msg, '\0', sizeof(msg)); // 用以给指定内存块填充数据
//             ssize_t s = read(fd[0], msg, sizeof(msg)); // 这个是有符号的整型,size_t是无符号的整型
//             // size_t一般用来表示"适用于计量内存中可容纳的数据项目个数的无符号整数类型",因此通常用在数组下标和内存管理函数之类的函数
//             // ssize_t一般用来表示"可以被执行读写操作的数据块的大小"
//             printf("%s\n", msg);
//             j++;
//         }
//     }
//     else
//     {
//         perror("fork error\n");
//         return 2;
//     }
//     return 0;
// }


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int main()
{
    int fd[2];
    int ret = pipe(fd);
    if (ret == -1)
    {
        perror("pipe error\n");
        return 1;
    }
    pid_t id = fork(); // fork之后,如果是0则代表是当前是子进程,但是在父进程中这个id是子进程的pid
    if (id == 0)
    {
        int i = 0;
        close(fd[0]);
        const char* child = "I am child ";
        while (i < 10)
        {
            write(fd[1], child, strlen(child)+1);
            sleep(1);
            i++;
        }
    }
    else if (id > 0)
    {
        close(fd[1]);
        char msg[100];
        int status = 0;
        int j = 0;
        while (j < 10)
        {
            memset(msg, '\0', sizeof(msg));
            ssize_t s = read(fd[0], msg, sizeof(msg));
            printf("%s %d\n", msg, j);
            j++;
        }
        close(fd[0]);
        pid_t ret = waitpid(id, &status, 0);// pid waitpid(pid_t pid, int* status, int options)
        // 暂停当前进程的执行,等待有信号来到或者子进程结束,如果没有这行,那么子进程还没有运行结束,父进程就退出了
        // 子进程的状态会存储在status中,如果只是当存想到阻塞功能,那么直接传入NULL即可

        // pid < -1: 等待进程组识别码为pid绝对值的任何子进程
        // pid = -1: 等待任何子进程,相当于wait()
        // pid = 0: 等待进程组识别码与目前进程相同的任何子进程
        // pid > 0: 等待任何子进程识别码为pid的子进程

        printf("exitsingle(%d), exit(%d)\n", status&0xff, (status>>8)&0xff);
        // 输出为13, 通过 kill -l 可以知道是 SIGPIPE 信号
    }
    else
    {
        perror("fork error\n");
        return 2;
    }
    return 0;
    // https://blog.csdn.net/skyroben/article/details/71513385
}