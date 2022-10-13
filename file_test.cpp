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
1. fopen：C/C++下的标准IO库函数，带输入/输出缓冲，在linux平台，该函数的底层实现就是open函数，因此这个函数具有可移植性
与open的最大区别是该函数只能处理普通文件，且以流形式处理。而open可以处理普通文件和设备文件
open函数返回的是文件句柄，而fopen返回的是文件指针，即指向FILE结构体的指针

头文件：#include <stdio.h>
函数原型： FILE * fopen(const char * path, const char * mode);
参数：
const char *path：文件路径
const char *mode：操作文件方式，可选有：
“r"或"rb” 以只读方式打开文件，该文件必须存在。
“w"或"wb” 以写方式打开文件，并把文件长度截短为零。
“a"或"ab” 以写方式打开文件，新内容追加在文件尾。
"r+"或"rb+“或"r+b” 以更新方式打开（读和写）
"w+"或"wb+“或"w+b” 以更新方式打开,并把文件长度截短为零。
"a+"或"ab+“或"a+b” 以更新方式打开，新内容追加在文件尾。
字母b表示文件时一个二进制文件而不是文本文件。（linux下不区分二进制文件和文本文件）

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
*/

/*
写入内容
*/
int main()
{
    FILE* fd = fopen("/home/rxsi/hello_world.txt", "w");
    char a[] = "aaaaaaaaa\n";
    char b[] = "bbbbbbbbb\n";
    char c[] = "ccccccccc\n";
    char d[] = "ddddddddd\n";
    char e[] = "eeeeeeeee\n";
    char f[] = "fffffffff\n";
    fwrite(a, 1, sizeof(a), fd);
    fwrite(b, 1, sizeof(a), fd);
    fwrite(c, 1, sizeof(a), fd);
    fwrite(d, 1, sizeof(a), fd);
    fwrite(e, 1, sizeof(a), fd);
    fwrite(f, 1, sizeof(a), fd);
    // fwrite(a, sizeof(a), 1, fd);
    // fwrite(b, sizeof(b), 1, fd);
    // fwrite(c, sizeof(c), 1, fd);
    // fwrite(d, sizeof(d), 1, fd);
    // fwrite(e, sizeof(e), 1, fd);
    // fwrite(f, sizeof(f), 1, fd);
    fclose(fd);
}


// size_t getFileSize(const char* filename)
// {
//     struct stat st;
//     stat(filename, &st);
//     return st.st_size;
// }

//  /*
// 1. 多线程读
// */
// void readFunc(int fd)
// {
//     char buf[10] = {0};
//     int i = 5;
//     while (i--)
//     {
//         ssize_t len = read(fd, buf, 10);
//         std::cout << "threadID: " << std::this_thread::get_id() << ", data_len: " << len << ", data: " << buf << std::endl;
//         std::cout << "tell: " << tellg(fd) << std::endl;
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }
// }

// int main()
// {
//     int fd = open("/home/rxsi/hello_world.txt", O_RDONLY);
//     std::thread t1(readFunc, std::ref(fd));
//     std::thread t2(readFunc, std::ref(fd));
//     t1.join();
//     t2.join();
//     close(fd);
// }

// /*
// 2. 多线程write
// */
