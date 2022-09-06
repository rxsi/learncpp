#include <stdio.h>
#include <mysql.h>
#include <iostream>

#define HOST "localhost"
#define USER "root"
#define PASSWRD "471653315"
#define DATABASE "test"
#define PORT 3306

int main()
{
    MYSQL mysql;
    /*
    MYSQL *
    mysql_init(MYSQL *mysql)
    如果MYSQL *mysql参数是NULL，那么在内部会创建并初始化一个MYSQL结构体
    */
    mysql_init(&mysql);
    /*
    MYSQL *
    mysql_real_connect(MYSQL *mysql,
                   const char *host,
                   const char *user,
                   const char *passwd,
                   const char *db,
                   unsigned int port,
                   const char *unix_socket,
                   unsigned long client_flag)
    用以连接数据库
    MYSQL *mysql: 已经创建好的MYSQL结构体，需要使用mysql_init()进行初始化，可通过mysql_options()设置该结构体的属性
    const char *host: host名或者具体的IP地址，如果该值是NULL或者"localhost"，那么将会连接本地的数据库
    const char *user: 用户名
    const char *passwd: 密码，这个密码不要加密
    const char *db: 数据库名，如果这个值是NULL，那么连接默认的数据库
    unsigned int port: 端口号，填0即可
    const char *unix_socket: 填 NULL 即可
    unsigned long client_flag: 一般填0即可，有多个可选参数
    */
    if (mysql_real_connect(&mysql, HOST, USER, PASSWRD, DATABASE, PORT, NULL, 0))
    {
        std::cout << "connect success " << endl;
        /*
        int
        mysql_query(MYSQL *mysql,
            const char *stmt_str)
        用以执行SQL语句
        const char *stmt_str: SQL语句，注意末尾不要加上";"符号。如果执行语句是二进制数据，那么需要使用mysql_read_query()函数。这是因为二进制数据会包含'\0'
        返回0代表查询成功
        */
        if (mysql_query(&mysql, "select * from test_table"))
        {
            std::cout << "query err" << std::endl;
            return 0;
        }
        /*
        MYSQL_RES *
        mysql_store_result(MYSQL *mysql)
        用以同步获取sql语句执行结果，使用mysql_store_result_nonblocking()则是异步版本
        此语句的执行是要在mysql_real_query或者mysql_query之后
        */
        MYSQL_RES* res = mysql_store_result(&mysql);
        if (res)
        {
            /*
            uint64_t
            mysql_num_rows(MYSQL_RES *result)
            用以获取结果集的函数
            如果上个调用的结果集方法是mysql_store_result，那么可以调用该函数；
            如果结果集方法是mysql_use_result，那么此处要使用mysql_fetch_row
            */
            int row_num = mysql_num_rows(res);
            /*
            unsigned int
            mysql_num_fields(MYSQL_RES *result)
            返回结果集中列的数量
            */
            int col_num = mysql_num_fields(res);
            MYSQL_FIELD *fd;
            /*
            MYSQL_FIELD *
            mysql_fetch_field(MYSQL_RES *result)
            返回结果集中定义的列
            */
            std::cout << "field name: ";
            while (fd = mysql_fetch_field(res))
            {
                std::cout << fd->name << "\t";
            }
            std::cout << endl;
            MYSQL_ROW sql_row;
            /*
            MYSQL_ROW
            mysql_fetch_row(MYSQL_RES *result)
            每调用一次返回一行的内容，因此需要使用while去循环调用，当没有内容时会返回NULL
            这个方法是同步的，mysql_fetch_row_nonblocking则是异步版本
            */
            while (sql_row = mysql_fetch_row(res))
            {
                for (int i = 0; i < col_num; ++i)
                {
                    if (sql_row[i] == NULL) std::cout << "NULL\t";
                    else std::cout << sql_row[i] << "\t"; 
                }
                std::cout << std::endl;
            }
        }
        /*
        void
        mysql_free_result(MYSQL_RES *result)
        释放结果集的内存
        */
        if (res != NULL) mysql_free_result(res);
    }
    else
    {
        std::cout << "connect err" << std::endl;
    }
    /*
    void
    mysql_close(MYSQL *mysql)
    关闭之前打开的连接
    */
    mysql_close(&mysql);
    /*
    void
    mysql_library_end(void)
    释放mysql包内存
    */
    mysql_library_end();
}