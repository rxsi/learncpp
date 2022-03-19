/*
位图数组测试
*/

// 示例要存储 1, 2, 5, 7, 11 ,最大值为11,因此至少需要11位,而一个字节对应8位,因此需要2个字节.
#include <iostream>
#include <bitset>

int main()
{
    int nums[] = {1, 2, 5, 7, 11};
    char bytes[2]; // 共16位

    for (auto& num : nums)
    {
        int index = num / 8;
        bytes[index] |= (1 << (num % 8));
    }

    for (auto& byte : bytes)
    {
        std::bitset<8> b(byte);
        std::cout << "byte = " << b.to_string() << std::endl;
        /*
            byte = 10100110
            byte = 00001000
        */
    }
}