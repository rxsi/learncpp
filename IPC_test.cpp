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


// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>
// #include <errno.h>
// #include <sys/wait.h>

// int main()
// {
//     int fd[2];
//     int ret = pipe(fd);
//     if (ret == -1)
//     {
//         perror("pipe error\n");
//         return 1;
//     }
//     pid_t id = fork(); // fork之后,如果是0则代表是当前是子进程,但是在父进程中这个id是子进程的pid
//     if (id == 0)
//     {
//         int i = 0;
//         close(fd[0]);
//         const char* child = "I am child ";
//         while (i < 10)
//         {
//             write(fd[1], child, strlen(child)+1);
//             sleep(1);
//             i++;
//         }
//     }
//     else if (id > 0)
//     {
//         close(fd[1]);
//         char msg[100];
//         int status = 0;
//         int j = 0;
//         while (j < 10)
//         {
//             memset(msg, '\0', sizeof(msg));
//             ssize_t s = read(fd[0], msg, sizeof(msg));
//             printf("%s %d\n", msg, j);
//             j++;
//         }
//         close(fd[0]);
//         pid_t ret = waitpid(id, &status, 0);// pid waitpid(pid_t pid, int* status, int options)
//         // 暂停当前进程的执行,等待有信号来到或者子进程结束,如果没有这行,那么子进程还没有运行结束,父进程就退出了
//         // 子进程的状态会存储在status中,如果只是当存想到阻塞功能,那么直接传入NULL即可

//         // pid < -1: 等待进程组识别码为pid绝对值的任何子进程
//         // pid = -1: 等待任何子进程,相当于wait()
//         // pid = 0: 等待进程组识别码与目前进程相同的任何子进程
//         // pid > 0: 等待任何子进程识别码为pid的子进程

//         printf("exitsingle(%d), exit(%d)\n", status&0xff, (status>>8)&0xff);
//         // 输出为13, 通过 kill -l 可以知道是 SIGPIPE 信号
//     }
//     else
//     {
//         perror("fork error\n");
//         return 2;
//     }
//     return 0;
//     // https://blog.csdn.net/skyroben/article/details/71513385
// }

/*
有名管道(FIFO)
FIFO为一种Unix系统的特殊文件类型,因为linxu中所有事物都是文件,他在文件系统中以文件名的形式存在.
在文件系统中有对应的文件路径,当一个进程以读(r)的方式打开该文件,而另一个进程以写(w)的方式打开该文件,
那么内核就会在这两个进程之间建立管道,所以FIFO实际上也由内核管理,并不与硬盘打交道.
当删除FIFO文件时,管道连接也随之消失.
FIFO的好处在于可以通过文件的路径来识别管道,从而让没有亲缘关系的进程之间建立连接

函数原型:
int mkfifo(const char* pathname, mod_t mode);
int mknode(const char* pathname, mode_t mode | S_IFIFO, (dev_t)0);
pathname 是一个普通的Unix路径名,也是该FIFO的名字
mode参数指定了文件权限和将被创建的文件类型(再次情况下是S_IFIFO),dev是创建设备特殊文件时使用的一个值,对于
先进先出文件该值为0
当创建一个FIFO后,他必须以只读方式打开或者只写方式打开,FIFO是半双工
一般的IO函数,如read, write, close, unlink都可以应用于FIFO
管道在所有进程最终关闭之后自动消失,或者主动调用close函数.而文件系统中的FIFO文件则需要通过调用unlink函数进行删除,否则下次调用mkfile会报错

*/

// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>
// #include <errno.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>

// #define PATH "/tmp/my_fifo"

// int main()
// {
//     int ret = mkfifo(PATH, 0777); // 如果文件已经存在,那么会返回-1
//     if (ret == -1)
//     {
//         perror("pipe error\n");
//         return 1;
//     }

//     pid_t id = fork();
//     if (id == 0)
//     {
//         int fd = open(PATH, O_WRONLY);
//         int i = 0;
//         const char* child = "I am child by fifo";
//         while (i < 5)
//         {
//             write(fd, child, strlen(child)+1);
//             sleep(1);
//             i++;
//         }
//         close(fd);
//     }
//     else if (id > 0)
//     {
//         int fd = open(PATH, O_RDONLY);
//         char msg[100];
//         int status = 0;
//         int j = 0;
//         while (j < 5)
//         {
//             memset(msg, '\0', sizeof(msg));
//             ssize_t s = read(fd, msg, sizeof(msg));
//             printf("%s %d\n", msg, j);
//             j++;
//         }
//         close(fd); // 关闭管道文件
//         unlink(PATH); // 删除管道文件,底层使用了引用计数,即使这个语句放在open之后就调用,依然不会影响已经打开的FIFO管道.
//         // 如果放在open之前,则open函数会被阻塞
//     }
//     else
//     {
//         perror("fork error\n");
//         return 2;
//     }
//     return 0;
// }

/*
信号
使用kill -l可以查看所有的信号
*/
// #include <stdio.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <sys/wait.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <cstdlib>

// int main()
// {
//     pid_t pid;
//     int ret;
//     pid = fork();
//     int newret;
//     if (pid < 0)
//     {
//         perror("fork error\n");
//         exit(1);
//     }
//     else if (pid == 0)
//     {
//         raise(SIGSTOP); // 使子进程进入暂停状态
//         // sleep(5);
//         exit(0);
//     }
//     else
//     {
//         printf("child process pid = %d\n", pid);
//         if (waitpid(pid, NULL, WNOHANG) == 0) // 设置为WHOHANG时,只要子进程没有dead,则会返回0;可以使子进程暂停,也可以使子进程sleep(5)
//         {
//             if (ret = kill(pid, SIGKILL) == 0) // 杀死子进程
//             {
//                 printf("kill child process");
//             }
//             else
//             {
//                 perror("kill child process fail");
//             }
//         }
//     }
// }

/*
消息队列
#include <sys/msg.h>
消息队列提供了一种从一个进程向另一个进程发送一个数据块的方法,消息队列是消息的链接表,存放在内核中并由消息队列标识符标识.
每个数据块都含有一个类型,接收进程可以独立的接收含有不同类型的数据结构,且具有最大长度限制
消息队列可以在没有接收进程的情况下,发送消息给消息队列,避免了管道/FIFO的同步和阻塞问题(管道类型要求至少存在一个读端和写端)

MSGMAX: 消息的最大长度
MSGMNB: 队列的最大长度
*/

/* msgget函数:用以打开一个现存的队列或者创建一个消息队列
int msgget(key_t key, int msgflg);
key_t: 本质是int类型
msgflg: 表示的权限标识,表示消息队列的访问权限.msgflg可以与IPC_CREATE做 | 操作,表示当key所命名的消息队列不存在时
创建一个消息队列,如果key命名的消息队列存在,则IPC_CREATE会被忽略,则只返回一个标识符(非0) 
如果创建失败,则返回-1.

如果成功创建了一个新的队列,那么会关联到一个结构体msqid_ds,通过该结构体可以控制消息队列的行为


struct msqid_ds
{
    struct ipc_perm msg_perm; // 权限设置,类似文件访问权限
    time_t msg_stime; // 最后调用msgsnd调用时间
    time_t msg_rtime; // 最后调用msgrcv调用时间
    time_t msg_ctime; // 队列的创建时间或者最后调用msgctl执行IPC_SET操作

    unsigned long msg_cbytes; // 当前队列中所有消息的总长度
    msgqnum_t msg_qnum; // 当前队列中有多少个消息
    msglen_t msg_qbytes; // 当前队列能够存放的消息体的总字节大小
    pid_t msg_lspid; // 最后调用msgsnd的进程的ID
    pid_t msg_lrpid; // 最后调用msgrcv的进程的ID
};

struct ipc_perm
{
    key_t __key; // 通过msgget创建的key
    uid_t uid; // 拥有者的id,可通过IPC_SET修改
    gid_t gid; // 拥有者的组id,可通过IPC_SET修改
    uid_t cuid; // 创建者的id
    gid_t cgid; // 拥有者的组id
    unsigned short mode; // 权限,可通过IPC_SET修改
    // mode可选有:
    // 0400 用户有读权限
    // 0200 用户有写权限
    // 0040 与用户同组的用户都有读权限
    // 0020 与用户同组的用户都有写权限
    // 0004 其他用户有读权限
    // 0002 其他用户有写权限
    
    // 如果设置0666,则代表所有用户都有可读可写权限,例如通过msgget(key_t key, 0666|IPC_CREATE)的方式创建并设置权限
    unsigned short __seq; 
};

*/

/*
msgctl函数:用以控制队列的权限和行为
int msgctl(int msqid, int cmd, struct msqid_ds* buf);
msqid:在msgget创建的id
cmd可选值:
IPC_STAT: 将在msgget创建的key 和 msqid_ds结构体关联信息,复制到buf中.不过调用次方法的用户要有读权限
IPC_SET:可设置某些属性
IPC_RMID:删除消息队列,会唤醒所有的读进程和写进程,给这些进程返回errno=EIDRM,msgctl的buf参数可以设置为NULL

当调用成功会返回0,如果失败则返回-1,且errno会被设置为对应的状态
*/

/*
msgsnd函数:发送消息到消息队列,调用者需要有写权限
int msgsnd(int msqid, const void* msgp, size_t msgz, int msgflg);
msgrcv函数:从消息队列中取出消息,调用者需要有读权限
ssize_t msgrcv(int msqid, void* msgp, size_t msgsz, long msgtyp, int msgflg);



void* msgp 指向结构体:
struct msgbuf
{
    long mtype; // 消息类型,需要>0
    char mtext[1]; // 消息体,该大小由size_t msgz指定
}
当插入的消息:
1)总字节数超过msg_qbytes字段
2)或者总消息数量超过msg_qbytes字段,这个判断是为了避免无限插入长度为0的消息

阻塞和非阻塞:
如果消息队列空间不足,那么msgsnd()会阻塞到空间足够,如果msgsnd函数的msgflg有设置IPC_NOWAIT,那么会立即返回失败,错误码errno = EAGAIN
当msgsnd为阻塞时,如果队列被移除(errno = EIDRM)或者被信号打断,如(errno = EINTR),那么会返回失败

当读取成功时,返回0,否则返回-1

msgrcv函数的msgsz参数如果小于所要接收的消息的mtext长度,则如果设置了MSG_NOERROR则会截断该消息,否则读取失败返回-1,
并且errno = E2BIG,原消息不会被移除.

如果 msgtyp == 0:则消息队列中的首个消息被读取
> 0时,消息队列中的msgbuf中与之相等的mtype的消息会被读取
< 0时,消息队列中第一个小于等于该绝对值的消息会被读取(实现对一个范围的消息进行读取)

如果消息队列中已经没有目标类型消息,当msgflg没有设置IPC_NOWAIT时,读取操作将会进入阻塞,指导有目标消息类型
的消息被放入消息队列中,或者该消息队列被移除.

当读取成功时,msgrcv函数返回对应的字节数,否则返回-1
*/

// #include <sys/msg.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <string.h>
// #include <iostream>

// #define MAX_TEXT_SIZE 50

// struct msg_buf
// {
//     long mtype;
//     char mtext[MAX_TEXT_SIZE];
// };

// int main()
// {

//     int msgid = msgget((size_t)123, 0666 | IPC_CREAT);
//     if (msgid == -1)
//     {
//         perror("msgget error");
//         return 1; // 通过return的方式是把栈弹出,回到上层调用,这里在main函数起到退出进程的效果
//     }

//     pid_t pid = fork();
//     if (pid < 0)
//     {
//         printf("fork error");
//         exit(0); // exit(0)是正常退出程序,exit(1)是非正常退出程序.exit的退出会直接删除进程,并将信号返回到OS
//     }

//     if (pid == 0)
//     {
//         for (int i = 1; i < 10; ++i)
//         {
//             msg_buf buf;
//             sleep(1);
//             char text[MAX_TEXT_SIZE] = "write msg from child process!" ;
//             buf.mtype = i; // 注意这里不能是0!!!!
//             strcpy(buf.mtext, text); // 注意这里不能是buf.mtext = text;
//             // 因为数组名在C语言中退化为常量指针,因此不能使用=号
//             // 只能使用memcpy(字符数组是strcpy),或者使用循环赋值的方式

//             if (msgsnd(msgid, &buf, MAX_TEXT_SIZE, 0) == -1)
//             {
//                 perror("msgsnd failed");
//                 exit(1);
//             }
//         }
//     }
//     else
//     {
//         msg_buf buf;
//         for (int i = 1; i < 10; ++i)
//         {
//             if (msgrcv(msgid, (void*)&buf, MAX_TEXT_SIZE, i, 0) == -1) // 这里设置i不为0,因此会接收对应的消息序号,如果是0,则每次都接收消息队列的头一个
//             {
//                 perror("msgrcv fail");
//                 exit(1);
//             }
//             printf("get the msg: %s\n", buf.mtext);
//         }
//         if (msgctl(msgid, IPC_RMID, 0) == -1)
//         {
//             perror("msgctl fail");
//             exit(1);
//         }
//         exit(0);
//     }
// }

// posix消息队列
// #include <stdio.h>
// #include <mqueue.h>
// #include <sys/stat.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <algorithm>

// #define PATH "/tmp" // 必须以/开头且只能含有一个/  如果想要指定诸如 /tmp/my_mq 这种格式,则需要使用 mount 挂载路径


// int main()
// {

//     mqd_t msgid;
//     int text_size = 50;
//     msgid = mq_open(PATH, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, NULL); // 创建消息队列
//     if (msgid == (mqd_t)-1)
//     {
//         perror("mq_open error");
//         exit(1);
//     }

//     mq_attr msg_attr;
//     if (mq_getattr(msgid, &msg_attr) == -1) // 获取消息队列属性
//     {
//         perror("mq_getattr error");
//         exit(1);
//     }


//     pid_t pid = fork();
//     if (pid < 0)
//     {
//         printf("fork error");
//         exit(0); // exit(0)是正常退出程序,exit(1)是非正常退出程序.exit的退出会直接删除进程,并将信号返回到OS
//     }

//     if (pid == 0)
//     {
//         for (int i = 1; i < 10; ++i)
//         {
//             sleep(1);
//             char text[text_size] = "write msg from child process!" ;
//             if (mq_send(msgid, (char*)&text, text_size, i) == -1)
//             {
//                 perror("mq_send error");
//                 exit(1);
//             }
//         }
//     }
//     else
//     {
//         int size = std::max(text_size, (int)msg_attr.mq_msgsize);
//         printf("size = %d\n", size); // 默认是8192
//         for (int i = 1; i < 10; ++i)
//         {
//             char buf[text_size];
//             unsigned int priop; // 存放优先级的指针
//             if (mq_receive(msgid, (char*)&buf, size, &priop) == -1)
//             {
//                 perror("mq_receive error");
//                 exit(1);
//             }
//             printf("get the msg: %s, priority: %d\n", buf, priop);
//         }

//         if (mq_close(msgid) == -1)
//         {
//             perror("mq_close error");
//             exit(1);
//         }

//         if (mq_unlink(PATH) == -1)
//         {
//             perror("mq_unlink error");
//             exit(1);
//         }
//     }
//     return 0;
// }


/*
mmap内存共享
*/
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <iostream>
#include <string.h>

#define PATH "/tmp/mmap_file"
#define SEM_PATH "/tmp"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
int SIZE = 100;

int main()
{
    char* ptr = (char*)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); // 当指定了MAP_ANONYMOUS属性时，代表不需要使用file，因此fd填入-1即可，用在有亲缘关系的进程间的通信
    pid_t pid = fork();
    if (pid == 0)
    {
        char s[SIZE] = "child string";
        memcpy(ptr, s, SIZE);
    }
    else if (pid > 0)
    {
        sleep(1); // 时子进程先写入
        char* ret = new char[SIZE];
        memcpy(ret, ptr, SIZE);
        std::cout << ret << std::endl;
        munmap(ptr, SIZE);
    }
    return 0;
}