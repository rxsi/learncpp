/*
多线程对同一个文件的修改
*/


#include <thread> // thread
#include <sys/types.h> // open, read
#include <sys/stat.h> // open, read
#include <fcntl.h> // open, read
#include <iostream> // cout, cin
#include <unistd.h> // read, close

/*
写入内容
*/
// int main()
// {
//     int fd = open("/home/rxsi/hello_world.txt", O_RDWR);
//     char a[10] = "aaaaaaaaa";
//     char b[10] = "bbbbbbbbb";
//     char c[10] = "ccccccccc";
//     char d[10] = "ddddddddd";
//     char e[10] = "eeeeeeeee";
//     char f[10] = "fffffffff";
//     write(fd, a, 10);
//     write(fd, b, 10);
//     write(fd, c, 10);
//     write(fd, d, 10);
//     write(fd, e, 10);
//     write(fd, f, 10);
//     close(fd);
// }

 /*
1. 多线程读
*/
void readFunc(int fd)
{
    char buf[10] = {0};
    int i = 5;
    while (i--)
    {
        ssize_t len = read(fd, buf, 10);
        std::cout << "threadID: " << std::this_thread::get_id() << ", data_len: " << len << ", data: " << buf << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    int fd = open("/home/rxsi/hello_world.txt", O_RDONLY);
    std::thread t1(readFunc, std::ref(fd));
    std::thread t2(readFunc, std::ref(fd));
    t1.join();
    t2.join();
    close(fd);
}

/*
2. 多线程write
*/

