#include<exception>
#include<stdexcept>
/*
exception是所有异常的父类
try{

}catch(...){ // 可以匹配所有异常并忽略

}

某些异常类可以接收C风格字符串或者string类型的参数
*/
// 定义自己的异常类
class out_of_block: public std::runtime_error
{
public:
    explicit out_of_block(const std::string &s): std::runtime_error(s){}
};