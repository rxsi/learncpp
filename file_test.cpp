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
#include <unistd.h> // read, close
 
void readFunc(int fd)
{
    char buf[12] = {0};
    int i = 5;
    while (i-- > 0)
    {
        ssize_t len = read(fd, buf, 12);
        std::cout << "threadID: " << std::this_thread::get_id() << ", data_len: " << len << ", data: " << buf << std::endl;
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