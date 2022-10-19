/*
多线程对同一个文件的修改
TODO：测试是否是同个进程内的多线程的write，read是原子性的，而多个进程之间则不是
*/


#include <thread> // thread
#include <sys/types.h> // open, read
#include <sys/stat.h> // open, read
#include <fcntl.h> // open, read
#include <iostream> // cout, cin
#include <unistd.h> // read, close
#include <fstream>
#include <sys/file.h>
#include <sys/wait.h>
#include <string.h>
#include <string>

/*
linux系统底层系统函数：
1. creat：创建文件
头文件：#include <sys/types.h>、#include <sys/stat.h>
函数原型：int creat(const char *filename, mode_t mode);
参数：
const char *filename：文件名
mode_t mode: 权限，和open函数同
一般来说这个函数比较少使用，因为open函数可以完成相同的工作，如果文件已经存在，则不会创建新文件

2. open：打开文件
头文件：#include<sys/types.h>、#include<sys/stat.h>、#include<fcntl.h>
函数原型：int open( const char * pathname, int oflags);
参数：
const char *pathname：文件路径+文件名
int oflags：文件的模式，有以下几个选项：
O_RDONLY: 只读模式
O_WRONLY: 只写模式
O_RDWR: 读写模式
这三个模式是互斥的

常见可选项：
O_APPEND: 追加模式
O_CREAT: 文件不存在则创建
O_EXCL: 如果已存在则返回-1
O_TRUNC: 文件已存在且是只写或读写则先清空文件内容
O_NONBLOCK: 非阻塞模式

int open( const char * pathname,int oflags, mode_t mode);
如果oflags指定了O_CREAT标志，那么mode_t mode参数就需要填写，用以设置新建文件的属性，常见有：
S_IRUSR: 读权限，文件属主
S_IWUSR: 写权限，文件属主
S_IXUSR: 执行权限，文件属主
S_IRGRP: 读权限，文件所属组
S_IWGRP: 写权限，文件所属组
S_IXGRP: 执行权限，文件所属组
S_IROTH: 读权限，其它用户
S_IWOTH: 写权限，其它用户
S_IXOTH: 执行权限，其它用户

3. close：关闭文件
头文件：#include <unistd.h>
函数原型：int close(int filedes)
参数：
int filedes：文件句柄

进程关闭之后，系统会自动关闭由这个进程打开的所有文件，不过严格的开发流程还是需要主动调用close函数
此外一个进程可以打开的文件套接字是有上限的，通过ulimit -n指令查看，默认是1024，所以合理的释放已经不使用的套接字是合理的

4. write：写文件
头文件：#include <unistd.h>
函数原型：ssize_t write(int filedes, const void *buf, size_t nbytes)
参数：
int filedes: 文件句柄
const void *buf：字节序列，一般使用char buf[] = "xxx" 这种方式
size_t nbytes：字节数，一般使用sizeof(buf)

这个函数具有原子性，但是多个进程同时对一个文件进行写是无法保证写入顺序的！！！！！，只有在同个进程内的多个线程才能保证写入的顺序
一般有这个需要则需要使用O_APPEND属性
而只有操作pipe、fifo下的文件大小小于PIPE_BUF的数据才可以在多进程同时进行时，保证绝对的保证原子性

5. read：读文件
头文件：#include <unistd.h>
函数原型：ssize_t read(int filedes, void *buf, size_t nbytes);
参数：
int filedes: 文件句柄
void *buf：用户态的缓存区，一般使用 char buf[N];
size_t nbytes：要读取的字节数，这个值一般等于小于N

这个函数具有原子性，但是如果多进程间同时读写时，会读取到别的进程写到一半的数据，因此要注意。一般多进程同时读写那么需要使用加锁的方式

6. lseek：修改文件偏移量，这是唯一可以修改文件偏移量的函数
头文件：#include <unistd.h>
函数原型：off_t lseek(int filedes, off_t offset, int whence);
int filedes：文件句柄
off_t offset：偏移量
int whence：决定了offset参数的含义，常见值有：
SEEK_SET：将文件偏移量修改为offset值
SEEK_CUR：将当前文件偏移量加上offset值，offset可正可负
SEEK_END：将文件偏移量设置为文件长度加上offset值，offset可正可负

这个函数也可以用来获取当前的的偏移量，比如：off_t cur = lseek(fd, 0, SEEK_CUR);

7. fcntl：用来操作文件描述符的特性
头文件：#include <fcntl.h>
函数原型：int fcntl(int filedes, int cmd, int arg);
参数：
int filedes：文件句柄
int cmd：功能类型
F_DUPFD：复制一个现有的文件描述符
F_GETFD/F_SETFD：获取/设置文件描述符标记
F_GETFL/F_SETFL：获取/设置文件状态标记
F_GETLK/F_SETLK/F_SETLKW：获取/设置记录锁
int arg：新属性

设置失败则返回INVALID_FD，0则设置成功

这个函数的用法一般是先通过F_GETFL获取当前的属性，然后再或上所要设置的新属性，再通过F_SETFL设置回去
int oldSocketFlag = fcntl(listenfd, F_GETFL, 0); // 旧属性
int newSocketFlag = oldSocketFlag | O_NONBLOCK; // 新属性
fcntl(listenfd, F_SETFL, newSocketFlag) // 设置属性

C/C++平台的语句
带有缓冲区，因此性能效率是高于linux原生语句的
1. fopen：C/C++下的标准IO库函数，带输入/输出缓冲，在linux平台，该函数的底层实现就是open函数，因此这个函数具有可移植性
与open的最大区别是该函数只能处理普通文件，且以流形式处理。而open可以处理普通文件和设备文件
open函数返回的是文件句柄，而fopen返回的是文件指针，即指向FILE结构体的指针

头文件：#include <stdio.h>
函数原型： FILE * fopen(const char * path, const char * mode);
参数：
const char *path：文件路径
const char *mode：操作文件方式，可选有：
“r"或"rb” 以只读方式打开文件，该文件必须存在。
“w"或"wb” 以只写方式打开文件，如果文件不存在会新建文件，并把文件长度截短为零，清空原有的内容
“a"或"ab” 以只写方式打开文件，如果文件不存在会新建文件，不会清空原内容，新内容会追加在文件尾。（a是append的意思）
"r+"或"rb+“或"r+b” 以读+写的方式打开文件，该文件必须存在
"w+"或"wb+“或"w+b” 以读+写的方式打开文件，如果文件不存在会新建文件，并把文件长度截短为零，清空原有的内容
"a+"或"ab+“或"a+b” 以读+写的方式打开文件，如果文件不存在会新建文件，不会清空原内容，新内容会追加在文件尾。（a是append的意思）
字母b表示文件时一个二进制文件而不是文本文件。（linux下不区分二进制文件和文本文件，windows下文本文件是以\r\n结尾而二进制文件是以\n结尾）

成功则返回 FILE 结构体指针，否则返回NULL且会设置errno标识错误

2. fclose：关闭文件
头文件：#include <stdio.h>
函数原型：int fclose(FILE *stream);
参数：
FILE *stream：FILE结构体指针

如果关闭成功则返回0，否则返回EOF

3. fgetc：获取下一个无符号字符，并把偏移量向前移动
头文件：#include <stdio.h>
函数原型：int fgetc(FILE *stream)
参数：
FILE *stream：FILE结构体指针

该函数以无符号 char 强制转换为 int 的形式返回读取的字符，如果到达文件末尾或发生读错误，则返回 EOF。

4.  feof：判断当前偏移量是否达到了文件的最末尾
头文件: #include <stdio.h>
函数原型：int feof(FILE *stream)
参数：
FILE *stream：FILE结构体指针

这个函数可以用在while循环中判断fgetc是否已经达到了最末尾

5. fputc：向字节流写入一个无符号字符
头文件：#include <stdio.h>
函数原型：int fputc(int char, FILE *stream)
参数：
int char：要被写入的字符，这里填入的是实际字符对应的ASCII码，比如写入65，那么对应的是字符A
FILE *stream：FILE结构体指针

如果写入成功，则返回被写入的字符，而错误返回EOF并设置错误标识符

6. fgets：从指定流读取指定字节数的内容
头文件：#include <stdio.h>
函数原型：char *fgets(char *str, int n, FILE *stream)
参数：
char *str：用户态的缓冲区
int n：要读取的字符数
FILE *stream：FILE结构体指针

有三种返回情况：
1. 还没有读取到文件末尾，那么会返回和str相同的指针
2. 如果到达了文件末尾或者没有读取到任何字符，那么str包含了读取到的内容，但是返回空指针
3. 如果发生了错误，那么返回空指针

7. fwrite：将指定数据写入字节流
头文件：#include <stdio.h>
函数原型：size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
参数：
const void *ptr：要被写入的元素数组的指针，貌似只能写入char数组
size_t size：要被写入的每个元素的大小，以字节位单位
size_t nmemb：写入元素的个数
FILE *stream：FILE结构体指针

示例：
char a[] = "abcde";
fwrite(a, 1, sizeof(a), fd); // 一个字节，总共6个元素

8. fread: 从字节流读取指定大小的数据
头文件：#include <stdio.h>
函数原型：size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
参数：
void *ptr：用户态的缓冲区
size_t size：每个元素的大小，单位字节
size_t nmemb：元素个数
FILE *stream：FILE结构体指针

返回成功读取到的元素总数，如果该值与nmemb不相同，那么可能是发送了错误，或者到达了文件末尾

9. fseek：设置偏移量
头文件：#include <stdio.h>
函数原型：int fseek(FILE *stream, long int offset, int whence)
参数：
FILE *stream：FILE结构体指针
long int offset：偏移量
int whence：模式
SEEK_SET：将文件偏移量修改为offset值
SEEK_CUR：将当前文件偏移量加上offset值，offset可正可负
SEEK_END：将文件偏移量设置为文件长度加上offset值，offset可正可负

10. rewind：把偏移量设置到文件头
头文件：#include <stdio.h>
函数原型：void rewind(FILE *stream)
参数：
FILE *stream：FILE结构体指针

11. ftell：返回当前的偏移量
头文件：#include <stdio.h>
函数原型：long int ftell(FILE *stream)
参数：
FILE *stream：FILE结构体指针

12. 在这种模式下要设置各种属性，需要借助fileno函数，用以将FILE结构体转换为对应的fd文件描述符
fileno：返回指向FILE结构体的fd文件描述符
头文件：<stdio.h>
函数原型：int fileno(FILE *stream);
参数：
FILE *stream：FILE结构体指针

flock：对文件描述符添加文件锁
头文件：#include <sys/file.h>
函数原型：int flock(int fd, int operation);
参数：
int fd：文件描述符
int operation：锁类型，参数有：
LOCK_SH：设置一个共享锁
LOCK_EX：设置一个互斥锁
LOCK_UN：移除本进程添加的共享/互斥锁
该函数方法默认会阻塞直到加锁成功，可以使用LOCK_NB设置为非阻塞模式
返回0则代表成功，-1则加锁失败，因此可以使用while循环判断加锁是否成功，当使用dup()或fork()时不会继承该文件锁！！！
*/

/*
写入内容
*/
// int main()
// {
//     FILE* fd = fopen("/home/rxsi/hello_world.txt", "w");
//     char a[] = "aaaaaaaaa\n";
//     char b[] = "bbbbbbbbb\n";
//     char c[] = "ccccccccc\n";
//     char d[] = "ddddddddd\n";
//     char e[] = "eeeeeeeee\n";
//     char f[] = "fffffffff\n";
//     fwrite(a, 1, sizeof(a), fd);
//     fwrite(b, 1, sizeof(a), fd);
//     fwrite(c, 1, sizeof(a), fd);
//     fwrite(d, 1, sizeof(a), fd);
//     fwrite(e, 1, sizeof(a), fd);
//     fwrite(f, 1, sizeof(a), fd);
//     fclose(fd);
// }


// size_t getFileSize(const char* filename)
// {
//     struct stat st;
//     stat(filename, &st);
//     return st.st_size;
// }

 /*
1. 多线程读
*/
// void readFunc(FILE *stream)
// {
//     int i = 6;
//     char buf[11];
//     while (i--)
//     {
//         std::cout << "threadID: " << std::this_thread::get_id() << ", ";
//         std::cout << "before fread ftell: " << ftell(stream) << ", ";
//         /*
//         这里的fread会顺序的交替输出，证明了fread具有原子性，不会在读取的中间过程被线程切换
//         */ 
//         ssize_t len = fread(buf, 1, sizeof(buf), stream);
        
//         if (len == 0)
//         {
//             std::cout << "read emtpty data" << std::endl;
//             break;
//         }
//         std::cout << "data_len: " << len << ", data: " << buf << ", ";
//         int fd = fileno(stream);
//         std::cout << "after fread ftell: " << ftell(stream) << ", stream->_file: " << stream->_file << ", fd: " << fd << std::endl;
//     }
// }

// int main()
// {
//     FILE *stream = fopen("/home/rxsi/hello_world.txt", "r");
//     std::thread t1(readFunc, stream);
//     std::thread t2(readFunc, stream);
//     t1.join();
//     t2.join();
//     fclose(stream);
// }

/*
2. 多线程write
*/

// void writeFunc(FILE *stream, char (*buf)[10]) // char buf[]、char *buf、char buf[11]都会被转换为指针丢失了数组特性，因此如果要保留数组特性那么需要使用数组指针 char (*buf)[]
// {
//     int i = 200;
//     int fd = fileno(stream);
//     // struct flock lock;
//     // memset (&lock, 0, sizeof(lock));
//     // lock.l_whence = SEEK_SET;
//     // lock.l_start = 0;
//     // lock.l_len = 0;
//     while (i--)
//     {
//         int ret = flock(fd, LOCK_EX); // 阻塞加锁
//         if (ret != 0) std::cout << "lock fail" << std::endl;
//         // lock.l_type = F_WRLCK;

//         // fcntl(fd, F_SETLK, &lock);
//         fseek(stream, 0, SEEK_END); // 移动到文件尾
//         std::cout << "thredID: " << std::this_thread::get_id() << ", ftell: " << ftell(stream) << std::endl;
//         ssize_t len = fwrite(*buf, 1, sizeof(*buf), stream);
//         // fsync(fd); 使用fsync也没有用，依然无法同步
//         flock(fd, LOCK_UN);
//         // lock.l_type = F_UNLCK;
//         // fcntl(fd, F_SETLK, &lock);
//     }
// }


// int main()
// {
//     FILE *stream1 = fopen("/home/rxsi/hello_world.txt", "w+");
//     char buf1[] = "aaaaaaaaa";
//     std::thread t1(writeFunc, stream1, &buf1);
//     FILE *stream2 = fopen("/home/rxsi/hello_world.txt", "w+");
//     char buf2[] = "bbbbbbbbb";
//     std::thread t2(writeFunc, stream2, &buf2);
//     t1.join();
//     t2.join();
//     fclose(stream1);
//     fclose(stream2);
// }

// 使用非缓存版本则正常
// void writeFunc(int fd, char (*buf)[10]) // char buf[]、char *buf、char buf[11]都会被转换为指针丢失了数组特性，因此如果要保留数组特性那么需要使用数组指针 char (*buf)[]
// {
//     int i = 200;
//     while (i--)
//     {
//         int ret = flock(fd, LOCK_EX); // 阻塞加锁
//         if (ret != 0) std::cout << "lock fail" << std::endl;
//         lseek(fd, 0, SEEK_END);// 移动到文件尾
//         std::cout << "thredID: " << std::this_thread::get_id() << ", ftell: " << lseek(fd, 0, SEEK_CUR) << std::endl;
//         ssize_t len = write(fd, buf, sizeof(*buf));
//         flock(fd, LOCK_UN);
//     }
// }


// int main()
// {
//     int fd1 = open("/home/rxsi/hello_world.txt", O_WRONLY|O_TRUNC);
//     char buf1[] = "aaaaaaaaa";
//     std::thread t1(writeFunc, fd1, &buf1);
//     int fd2 = open("/home/rxsi/hello_world.txt", O_WRONLY|O_TRUNC);
//     char buf2[] = "bbbbbbbbb";
//     std::thread t2(writeFunc, fd2, &buf2);
//     t1.join();
//     t2.join();
//     close(fd1);
//     close(fd2);
// }

// 使用非缓存版本则正常，使用fcntl加锁
// void writeFunc(int fd, char (*buf)[10]) // char buf[]、char *buf、char buf[11]都会被转换为指针丢失了数组特性，因此如果要保留数组特性那么需要使用数组指针 char (*buf)[]
// {
//     int i = 200;
//     struct flock lock;
//     memset(&lock, 0, sizeof(lock));
//     lock.l_whence = SEEK_SET;
//     lock.l_start = 0;
//     lock.l_len = 0;
//     while (i--)
//     {
//         lock.l_type = F_WRLCK;
//         int ret = fcntl(fd, F_SETLKW, &lock); // 使用阻塞模式
//         if (ret != 0) std::cout << "lock fail" << std::endl;
//         lseek(fd, 0, SEEK_END);// 移动到文件尾
//         std::cout << "thredID: " << std::this_thread::get_id() << ", ftell: " << lseek(fd, 0, SEEK_CUR) << std::endl;
//         ssize_t len = write(fd, buf, sizeof(*buf));
//     }
//     // lock.l_type = F_UNLCK;
//     // fcntl(fd, F_SETLK, &lock);
// }


// int main()
// {
//     int fd1 = open("/home/rxsi/hello_world.txt", O_WRONLY|O_TRUNC);
//     char buf1[] = "aaaaaaaaa";
//     std::thread t1(writeFunc, fd1, &buf1);
//     int fd2 = open("/home/rxsi/hello_world.txt", O_WRONLY|O_TRUNC);
//     char buf2[] = "bbbbbbbbb";
//     std::thread t2(writeFunc, fd2, &buf2);
//     t1.join();
//     t2.join();
//     close(fd1);
//     close(fd2);
// }

/*
3. 多线程读+写
*/
// void readFunc(FILE *stream)
// {
//     int i = 200;
//     char buf[10];
//     while (i--)
//     {
//         std::cout << "threadID: " << std::this_thread::get_id() << ", ";
//         ssize_t len = fread(buf, 1, sizeof(buf), stream); // 这里每次都可以完整的读取一整行"aaaaaaaaa"，说明write和read是交替完成的
//         std::cout << "fread len: " << len << std::endl;
//         if (len != 0) std::cout << "data: " << buf << std::endl;
//     }
// }

// void writeFunc(FILE *stream, char (*buf)[10]) // char buf[]、char *buf、char buf[11]都会被转换为指针丢失了数组特性，因此如果要保留数组特性那么需要使用数组指针 char (*buf)[]
// {
//     int i = 200;
//     while (i--)
//     {
//         ssize_t len = fwrite(*buf, 1, sizeof(*buf), stream);
//     }
// }


// int main()
// {
//     FILE *stream1 = fopen("/home/rxsi/hello_world.txt", "a+");
//     char buf1[] = "aaaaaaaaa";
//     std::thread t1(writeFunc, stream, &buf1);
//     std::thread t2(readFunc, stream, &buf1);
//     t1.join();
//     t2.join();
//     fclose(stream);
// }

// void readFunc(int fd)
// {
//     int i = 100;
//     while (i--)
//     {
//         std::cout << "processID: " << getpid() << ", ";
//         char buf[10];
//         std::cout << "before ftell: " << lseek(fd, 0, SEEK_CUR) << ", ";
//         size_t len = read(fd, buf, sizeof(buf));
//         std::cout << "len: " << len << ", ";
//         if (len == 0)
//         {
//             std::cout << "empty data" << ", ";
//         }
//         else
//         {
//             std::cout << "data: " << buf << ", "; 
//         }
//         std::cout << "after ftell: " << lseek(fd, 0, SEEK_CUR) << std::endl;
//     }
// }

// int main()
// {
//     // FILE *stream = fopen("/home/rxsi/hello_world.txt", "r");
//     int fd = open("/home/rxsi/hello_world.txt", O_RDONLY);
//     pid_t pid = fork();
//     if (pid == 0)
//     {
//         readFunc(fd);
//     }
//     else
//     {
//         readFunc(fd);
//         int status;
//         wait(&status);
//     }
// }

// void readFunc(std::string flag, FILE *stream)
// {
//     int i = 100;
//     while (i--)
//     {
//         std::cout << "flag: " << flag << ", processID: " << getpid() << ", ";
//         char buf[10];
//         std::cout << "before ftell: " << ftell(stream) << ", ";
//         size_t len = fread(buf, 1, sizeof(buf), stream);
//         std::cout << "len: " << len << ", ";
//         if (len == 0)
//         {
//             std::cout << "empty data" << ", ";
//         }
//         else
//         {
//             std::cout << "data: " << buf << ", "; 
//         }
//         std::cout << "after ftell: " << ftell(stream) << ", fd: " << fileno(stream) << std::endl;
//     }
// }

// int main()
// {
//     FILE *stream = fopen("/home/rxsi/hello_world.txt", "r");
//         readFunc("parent", stream);
//     pid_t pid = fork();
//     if (pid == 0)
//     {
//         readFunc("child", stream);
//     }
//     else
//     {
//         int status;
//         wait(&status);
//     }
// }



/*
1. 多进程读
*/

// void readFunc()
// {
//     FILE *stream = fopen("/home/rxsi/hello_world.txt", "r");
//     int i = 100;
//     while (i--)
//     {
//         std::cout << "processID: " << getpid() << ", ";
//         char buf[10];
//         std::cout << "before ftell: " << ftell(stream) << ", ";
//         size_t len = fread(buf, 1, sizeof(buf), stream); // 两个进程是交替输出的，而且ftell也不会交叉，因为两个进程的FILE结构是独立的
//         std::cout << "len: " << len << ", ";
//         if (len == 0)
//         {
//             std::cout << "empty data" << ", ";
//         }
//         else
//         {
//             std::cout << "data: " << buf << ", "; 
//         }
//         std::cout << "after ftell: " << ftell(stream) << std::endl;
//     }
// }

// int main()
// {
//     pid_t pid = fork();
//     if (pid == 0) // 子进程
//     {
//         readFunc();
//     }
//     else
//     {
//         readFunc();
//     }
// }


/*
2. 多进程写
*/
// 使用append方法
// void writeFunc(FILE *stream, char (*buf)[10]) // char buf[]、char *buf、char buf[11]都会被转换为指针丢失了数组特性，因此如果要保留数组特性那么需要使用数组指针 char (*buf)[]
// {
//     int i = 200;
//     while (i--)
//     {
//         /*
//         这里会交替写入200个a和b，使用 grep -c "aaaaaaaaa" file， grep -c "bbbbbbbbb" file 可以查看
//         */ 
//         ssize_t len = fwrite(*buf, 1, sizeof(*buf), stream);
//         std::cout << "processID: " << getpid() << ", ftell: " << ftell(stream) << std::endl;
//     }
// }

// int main()
// {
//     pid_t pid = fork();
//     if (pid == 0) // 子进程
//     {
//         // FILE *stream = fopen("/home/rxsi/hello_world.txt", "w"); // 以w方式打开之后两个进程会互相覆盖，结果是不可预知的，因此需要使用append的方式
//         FILE *stream = fopen("/home/rxsi/hello_world.txt", "a"); // 使用这种方式之后，底层每次在写之前都会获取最新的文件长度，然后再执行写入操作，因为本身操作具有原子性，因此就不会有互相覆盖问题了。
//         char buf[] = "aaaaaaaaa";
//         writeFunc(stream, &buf);
//     }
//     else
//     {
//         FILE *stream = fopen("/home/rxsi/hello_world.txt", "a"); 
//         char buf[] = "bbbbbbbbb";
//         writeFunc(stream, &buf);
//     }
// }

// 这里使用加文件锁的方式，会互相覆盖
// void writeFunc(FILE *stream, char (*buf)[10]) // char buf[]、char *buf、char buf[11]都会被转换为指针丢失了数组特性，因此如果要保留数组特性那么需要使用数组指针 char (*buf)[]
// {
//     int i = 200;
//     int fd = fileno(stream);
//     while (i--)
//     {
//         int ret = flock(fd, LOCK_EX);
//         if (ret != 0) std::cout << "lock err" << std::endl; // 简单处理，一般不会加锁失败
//         // while (flock(fd, LOCK_EX | LOCK_NB) != 0) {} // 使用while循环非阻塞加锁直到成功
//         fseek(stream, 0, SEEK_END); // 每次都移动到文件的末尾，保证两个进程不会互相覆盖
//         ssize_t len = fwrite(*buf, 1, sizeof(*buf), stream);
//         std::cout << "processID: " << getpid() << ", ftell: " << ftell(stream) << std::endl;
//         flock(fd, LOCK_UN);
//         // if (i == 0) flock(fd, LOCK_UN); // 通过这个方式可以使进程A先执行完再释放锁，因为flock如果fd已经持有锁则可重入，但是只需要解锁一次。
//     }
// }

// int main()
// {
//     pid_t pid = fork();
//     if (pid == 0) // 子进程
//     {
//         FILE *stream = fopen("/home/rxsi/hello_world.txt", "w"); 
//         char buf[] = "aaaaaaaaa";
//         writeFunc(stream, &buf);
//     }
//     else
//     {
//         FILE *stream = fopen("/home/rxsi/hello_world.txt", "w"); 
//         char buf[] = "bbbbbbbbb";
//         writeFunc(stream, &buf);
//         int status;
//         wait(&status);
//     }
// }

// 使用open版本不带缓存的会正常，而fopen则不正常，表现为ftell会获取到已经被写入的位置，怀疑是缓存未及时更新的原因
// void writeFunc(int fd, char (*buf)[10]) // char buf[]、char *buf、char buf[11]都会被转换为指针丢失了数组特性，因此如果要保留数组特性那么需要使用数组指针 char (*buf)[]
// {
//     int i = 200;
//     while (i--)
//     {
//         int ret = flock(fd, LOCK_EX);
//         if (ret != 0) std::cout << "lock err" << std::endl; // 简单处理，一般不会加锁失败
//         // while (flock(fd, LOCK_EX | LOCK_NB) != 0) {} // 使用while循环非阻塞加锁直到成功
//         lseek(fd, 0, SEEK_END);
//         // fseek(stream, 0, SEEK_END); // 每次都移动到文件的末尾，保证两个进程不会互相覆盖
//         ssize_t len = write(fd, buf, sizeof(*buf));
//         // ssize_t len = fwrite(*buf, 1, sizeof(*buf), stream);
//         std::cout << "processID: " << getpid() << ", ftell: " << lseek(fd, 0, SEEK_CUR) << std::endl;
//         flock(fd, LOCK_UN);
//         // if (i == 0) flock(fd, LOCK_UN); // 通过这个方式可以使进程A先执行完再释放锁，因为flock如果fd已经持有锁则可重入，但是只需要解锁一次。
//     }
// }

// int main()
// {
//     pid_t pid = fork();
//     if (pid == 0) // 子进程
//     {
//         int fd = open("/home/rxsi/hello_world.txt", O_WRONLY | O_TRUNC);
//         char buf[] = "aaaaaaaaa";
//         writeFunc(fd, &buf);
//     }
//     else
//     {
//         int fd = open("/home/rxsi/hello_world.txt", O_WRONLY | O_TRUNC);
//         char buf[] = "bbbbbbbbb";
//         writeFunc(fd, &buf);
//         int status = 0;
//         wait(&status);
//     }
// }

/*
3. 多进程写+读
*/

// 未加锁，那么会出现读取到别人写一半的内容
// void writeFunc(FILE *stream, char (*buf)[11]) // char buf[]、char *buf、char buf[11]都会被转换为指针丢失了数组特性，因此如果要保留数组特性那么需要使用数组指针 char (*buf)[]
// {
//     ssize_t len = fwrite(*buf, 1, sizeof(*buf), stream);
//     std::cout << "processID: " << getpid() << ", ftell: " << ftell(stream) << ", write success" << std::endl;
// }

// // 假设当前读取缓存区不足以一次性读取所有的数据，因此分了两次进行读取
// void readFunc(FILE *stream)
// {
//     int i = 10;
//     char buf[10];
//     int step = 0;
//     while (i--)
//     {
//         char temp[1] = {0};
//         size_t len = fread(temp, 1, sizeof(temp), stream);
//         std::cout << len << " , " << temp << " , " << sizeof(temp) << std::endl;
//         strcpy(buf+step, temp);
//         step += 1;
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }
//     std::cout << buf << std::endl;
// }

// int main()
// {
//     pid_t pid = fork();
//     if (pid == 0) // 子进程，先写入aaaaaaaaa，然后再写入bbbbbbbbb
//     {
//         FILE *stream = fopen("/home/rxsi/hello_world.txt", "w"); 
//         char buf1[] = "aaaaaaaaaa";
//         writeFunc(stream, &buf1); // 先写入了aaaaaaaaa
//         fseek(stream, 0, SEEK_SET); // 把文件偏移量设置回文件开头
//         char temp[10];
//         size_t len = fread(temp, 1, sizeof(temp), stream);
//         std::cout << temp << std::endl;
//         std::this_thread::sleep_for(std::chrono::seconds(5));
//         fseek(stream, 0, SEEK_SET); // 把文件偏移量设置回文件开头
//         char buf2[] = "bbbbbbbbbb";
//         writeFunc(stream, &buf2); // 再从头写入bbbbbbbbb
//     }
//     else
//     {
//         FILE *stream = fopen("/home/rxsi/hello_world.txt", "r");
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//         readFunc(stream);
//         int status = 0;
//         wait(&status);
//     }
// }


void writeFunc(int fd, char (*buf)[5]) // char buf[]、char *buf、char buf[11]都会被转换为指针丢失了数组特性，因此如果要保留数组特性那么需要使用数组指针 char (*buf)[]
{
    ssize_t len = write(fd, *buf, sizeof(*buf));
}

// 假设当前读取缓存区不足以一次性读取所有的数据，因此分了两次进行读取
void readFunc(int fd)
{
    int i = 5;
    char buf[5];
    int step = 0;
    char temp[256];
    size_t len = read(fd, temp, sizeof(temp));
    std::string s(temp);
    std::cout << "len: " << len << ", temp: " << temp << ", offset: " << lseek(fd, 0, SEEK_CUR) << std::endl;
    printf("data: %s", temp);
    // char temp2[4];
    // size_t len2 = read(fd, temp2, sizeof(temp2));
    // std::cout << "len2: " << len2 << ", temp: " << temp2 << ", offset: " << lseek(fd, 0, SEEK_CUR) << std::endl;
    // char temp3[4];
    // size_t len3 = read(fd, temp3, sizeof(temp3));
    // std::cout << "len3: " << len3 << ", temp: " << temp3 << ", offset: " << lseek(fd, 0, SEEK_CUR) << std::endl;
    // while (i--)
    // {
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    //     size_t len = read(fd, temp, sizeof(temp));
    //     std::string new_temp(temp);
    //     std::cout << new_temp << std::endl;
    //     strcpy(buf+step, temp);
    //     std::cout << "buf: " << buf << ", size: " << sizeof(buf) << std::endl;
    //     step += 1;
    // }
    // std::string s(buf);
    // std::cout << s << std::endl;
}

int main()
{
    pid_t pid = fork();
    if (pid == 0) // 子进程，先写入aaaaaaaaa，然后再写入bbbbbbbbb
    {
        int fd = open("/home/rxsi/hello_world.txt", O_WRONLY|O_TRUNC);
        for (int i = 0; i < 3; ++i)
        {
            char buf1[] = "aaaa";
            writeFunc(fd, &buf1); // 先写入了aaaaaaaaa
        }
        // std::this_thread::sleep_for(std::chrono::seconds(3));
        // lseek(fd, 0, SEEK_SET);
        // char buf2[] = "bbbbb";
        // writeFunc(fd, &buf2); // 再从头写入bbbbbbbbb
    }
    else
    {
        int fd = open("/home/rxsi/hello_world.txt", O_RDONLY);
        readFunc(fd);
        int status = 0;
        wait(&status);
    }
}


// 父子进程共享file结构
// 父进程先调用readFunc，后再由子进程继续调用readFunc，此时他们输出的ftell是连续的，因此证明了父子进程是共享file结构的。
// void readFunc(FILE *stream)
// {
//     int i = 100;
//     while (i--)
//     {
//         std::cout << "processID: " << getpid() << ", ";
//         char buf[10];
//         std::cout << "before ftell: " << ftell(stream) << ", ";
//         size_t len = fread(buf, 1, sizeof(buf), stream);
//         std::cout << "len: " << len << ", ";
//         if (len == 0)
//         {
//             std::cout << "empty data" << ", ";
//         }
//         else
//         {
//             std::cout << "data: " << buf << ", "; 
//         }
//         std::cout << "after ftell: " << ftell(stream) << std::endl;
//     }
// }

// int main()
// {
//     FILE *stream = fopen("/home/rxsi/hello_world.txt", "r");
//     readFunc(stream);
//     pid_t pid = fork();
//     if (pid == 0) // 子进程
//     {
//         readFunc(stream);
//     }
//     else
//     {
//         int status = 0;
//         wait(&status); // 等待子进程退出
//     }
// }

/*
测试flock&fcntl&lockf
*/