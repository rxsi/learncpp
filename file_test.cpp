/*
多线程对同一个文件的修改
*/
/*
1. 多线程读
*/

#include <thread> // thread
#include <sys/types.h> // open, read
#include <sys/stat.h> // open, read
#include <fcntl.h> // open, read
#include <iostream> // cout, cin
 
void readFunc(int fd)
{
    char buf[10] = {0};
    ssize_t len = read(fd, buf, 10);
    std::cout << "threadID: " << std::this_thread::get_id() << ", data: " << buf << std::endl;
}

int main()
{
    int fd = open("home/rxsi/hello_world.txt", O_RDONLY);
    std::thread t1(readFunc, std::ref(fd));
    std::thread t2(readFunc, std::ref(fd));

}