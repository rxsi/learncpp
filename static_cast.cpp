/*
static_cast 用来进行强制类型转换
*/
// 应用场景1:
#include<iostream>

class Base{
public:
    const char& operator[](std::size_t position) const{
        /*
        很多的重复代码
        */
        return text[position];
    }

    char& operator[](std::size_t position){ // 为避免无意义的重写代码,先将非const转为const,然后调用const operator[], 再将结果转换为非const
        return const_cast<char&>(
            static_cast<const Base&>(*this)[position]
        );
    }
private:
    char *text;
};

int main(){
    
}