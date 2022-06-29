#include <stdio.h>
// #include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
// #include <stdbool.h>
// #include <sys/stat.h>
// #include <sys/types.h>
#include <sys/mman.h>

#define SHM_NAME_LEN 1024
#define IS_POT(x) ((x) && !((x) & (x)-1))

template <typename T>
class LockFreeQueue
{
public:
    // size:目标队列的大小
    // name:共享内存key的路径名称,默认为NULL,即使用数组作为底层
    LockFreeQueue(unsigned int size, const char* name = NULL) // stdio.h
    {
        memset(shm_name, 0, sizeof(shm_name)); // string.h
        createQueue(name, size);
    }
blog.51cto.com/quantfabric/2588193
    ~LockFreeQueue()

protected:
    virtual void createQueue(const char* name, unsigned int size)
    {
#ifdef USE_POT
        if (!IS_POT(size))
        {
            size = roundup_pow_of_two(size);
        }
#endif
        m_size = size;
        m_head = m_tail = 0;
        if (name == NULL)
        {
            m_buffer = new T[m_size];
        }
        else
        {
            int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); // sys/mman.h, fcntl.h
            if (shm_fd < 0)
            {
                perror("shm_open err");
            }
            if (ftruncate(shm_fd, m_size * sizeof(T)) < 0) // unistd.h
            {
                perror("ftruncate err");
                close(shm_fd);
            }
            void* addr = mmap(0, m_size * sizeof(T), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
            if (addr == MAP_FAILED)
            {
                perror("mmap err");
                close(shm_fd);
            }
            if (close(shm_fd) == -1)
            {
                perror("close err");
                exit(1);
            }
            m_buffer = static_cast<T*>(addr);
            memcpy(shm_name, name, SHM_NAME_LEN - 1);
        }
#ifdef USE_LOCK
    spinlock_init(m_lock);
#endif
    }

    inline unsigned int roundup_pow_of_two(size_t size)
    {
        size |= size >> 1;
        size |= size >> 2;
        size |= size >> 4;
        size |= size >> 8;
        size |= size >> 16;
        size |= size >> 32;
        return size + 1;
    }

private:
    char shm_name[SHM_NAME_LEN];
    volatile unsigned int m_head;
    volatile unsigned int m_tail;
    unsigned int m_size;

#ifdef USE_LOCK
    spinlock_t m_spinLock;
#endif
    T* m_buffer;
};