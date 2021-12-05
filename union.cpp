/*

*/
#include<string>
using namespace std;

class Token
{
public:
    Token(): tok(INT), ival(0) {}
    Token(const Token &t): tok(t.tok) {copyUnion(t);}
    ~Token()
    {
        if (tok == STR)
        {
            sval.~string();
        }
    }

    Token &operator=(const Token&);
    Token &operator=(const string&);
    Token &operator=(char);
    Token &operator=(int);
    Token &operator=(double);

private:
    enum {INT, CHAR, DBL, STR} tok; // 判别式, tok定义在enum类结束之前,代表了tok就是一个enum实例.与普通类直接定义实例的规则一样
    union // 作为union组成部分的类成员是无法自动销毁的
    // 当外层类被销毁时,会自动调用析构函数,但是定义在union中的类成员因为外层类并不清除究竟union存储的值的类型,因此需要我们手动调用union中含有析构函数的类成员的析构函数
    {
        char cval;
        int ival;
        double dval;
        string sval;
    }; // union 本质是一个类,因此需要加;
    void  copyUnion(const Token&);
};

Token &Token::operator=(int i)
{
    if (tok == STR) sval.~string(); // 操作union要时刻记住!!!!!!!
    ival = i;
    tok = INT;
    return *this;
}

Token &Token::operator=(const string &s)
{
    if(tok == STR){
        sval = s; // 不需要先释放掉,因为我们直接使用拷贝的方式,替换原有值即可
    } 
    else
    {

    }
}