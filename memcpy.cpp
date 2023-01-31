#include <iostream>

void* memcpy(void* src, void* dest, size_t size)
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

int main()
{
    char src[] = "abcdefg";
    char dest[] = "hijklmnaaaaaaaaaa";
    // char dest[20];
    char* newDest = strncpy(dest, src, 13);
    // char* newDest = static_cast<char*>(memcpy(src, src+2, 3));
    std::cout << newDest << std::endl;
    // std::cout << *(newDest+12) << std::endl;
}