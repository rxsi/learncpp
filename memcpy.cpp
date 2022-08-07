#include <iostream>

void* memcpy(void* src, void* dest, size_t size)
{
    if (src == dest || size <= 0) return dest;
    char* psrc = static_cast<char*>(src);
    char* pdest = static_cast<char*>(dest);
    if (pdest > psrc && pdest < psrc + size)
    {
        // 说明二者存在互相覆盖问题，因此需要逆序
        pdest = pdest + size - 1;
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

int main()
{
    char src[] = "abcdefg";
    char dest[] = "hijklmn";
    char* newDest = static_cast<char*>(memcpy(src, src+1, 3));
    std::cout << newDest << std::endl;
}