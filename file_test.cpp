/*
多线程对同一个文件的修改
*/


#include <thread> // thread
#include <sys/types.h> // open, read
#include <sys/stat.h> // open, read
#include <fcntl.h> // open, read
#include <iostream> // cout, cin
#include <unistd.h> // read, close
#include <fstream>

/*
写入内容
*/
int main()
{
    FILE* fd = fopen("/home/rxsi/hello_world.txt", "w");
    char a[] = "aaaaaaaaa";
    char b[] = "bbbbbbbbb";
    char c[] = "ccccccccc";
    char d[] = "ddddddddd";
    char e[] = "eeeeeeeee";
    char f[] = "fffffffff";
    fwrite(a, sizeof(a), 1, fd);
    fwrite(b, sizeof(b), 1, fd);
    fwrite(c, sizeof(c), 1, fd);
    fwrite(d, sizeof(d), 1, fd);
    fwrite(e, sizeof(e), 1, fd);
    fwrite(f, sizeof(f), 1, fd);
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

