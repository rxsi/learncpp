// int a = 2; // 有同名的全局变量,连接失败
// extern int a; //外部链接性,声明
// static int a = 2; 内部链接性
const int a = 2; // 内部链接性

int main(){
    return 0;
}