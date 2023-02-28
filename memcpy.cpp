#include <iostream>
#include <assert.h>

void* memcpy(void* src, void* dest, size_t size) // 如果说这个复制效率慢了，那么可以一次性复制多个，比如8个字节8个字节的复制，当然前提是对齐
{
    if (src == dest || size <= 0) return dest;
    char* psrc = static_cast<char*>(src);
    char* pdest = static_cast<char*>(dest);
    if (pdest > psrc && pdest < psrc + size)
    {
        // 说明二者存在互相覆盖问题，因此需要逆序
        pdest = pdest + size - 1; // 注意这里要-1
        psrc = psrc + size - 1;
        while (size--)
        {
            *pdest-- = *psrc--;
        }
    }
    else
    {
        while (size--)
        {
            *pdest++ = *psrc++;
        }
    }
    return dest;
}

char* strcpy(char *dest, char *src)
{
    if (dest == src) return dest;
    char* destCpy = dest;
    while ((*dest++ = *src++) != '\0');
    return destCpy;
}

char* strncpy(char *dest, char *src, int n)
{
    if (dest == src || n <= 0) return dest;
    char *destCpy = dest;
    while (n-- && (*dest++ = *src++) != '\0');
    while (n-- > 0) *dest++ = '\0'; // 当长度不够时补'\0'
    return destCpy;
}


int atoi(const char *str) // 实现atoi
{
    assert(str != nullptr);
    int sign = 1; // 正负数，1为正，-1为负
    int result = 0;
    while (*str == ' ' || *str == '\n' || *str == '\t') ++str;
    if (*str == '-')
    {
        sign = -1;
        ++str;
    }
    if (*str == '+') ++str;
    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + *str - '0';
        ++str;
    }
    return sign * result;
}

char *itoa(int num, char *buf, unsigned radix) // 实现itoa
{
    char *p = buf;
    if (num < 0)
    {
        *p++ = '-';
        num = -num;
    }
    char *first = p;
    while (num > 0)
    {
        int tmp = num % radix;
        num /= radix;
        if (tmp > 9) *p++ = (char)(tmp - 10 + 'a');
        else *p++ = (char)(tmp + '0');
    }
    *p-- = '\0'; // p逆序存储了结果，因此我们现在最后的位置加上\0
    while (first < p) // first指向了第一个元素，p指向了最后的元素，因此进行互换
    {
        char tmp = *p;
        *p = *first;
        *first = tmp;
        first++;
        p--;
    }
    return buf;
}

int main()
{
    // char src[] = "abcdefg";
    // char dest[] = "hijklmnaaaaaaaaaa";
    // char dest[20];
    // char* newDest = strncpy(dest, src, 13);
    // char* newDest = static_cast<char*>(memcpy(src, src+2, 3));
    // std::cout << newDest << std::endl;
    // std::cout << *(newDest+12) << std::endl;
    // char str[] = "  23+4";
    // std::cout << atoi(str) << std::endl;
    char str2[20];
    int num = -23423;
    std::cout << itoa(num, str2, 16) std::endl;
    return 0;
}