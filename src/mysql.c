#include "../include/mysql.h"

//初始化MySQL
MYSQL * init_MySql(void)
{
    MYSQL *conn = NULL;
    char *host = "localhost";
    char *user = "root";
    char *passwd = "588686";
    char *db = "cloud_disk";

    //初始化MySQL链接的结构体
    conn = mysql_init(NULL);

    //建立连接
    if(mysql_real_connect(conn, host, user, passwd, db, 0, NULL, 0) == NULL){
        printf("Error: %s\n", mysql_error(conn));
        return NULL;
    }
    return conn;
}

//关闭MySQL
void destory_MySql(MYSQL *conn)
{
    mysql_close(conn);
}

//增加用户
int addUser_MySql(MYSQL *conn, char *userName, char *salt, char *crypass)
{
    //查询语句
    char query[512] = {0};
    sprintf(query, 
            "insert into user(username, salt, cryptpasswd, pwd)value('%s', '%s', '%s', '.');",
            userName, salt, crypass);
    int ret = mysql_query(conn, query);
    if(ret != 0){
        printf("Query Error: %s\n", mysql_error(conn));
        return -1;
    }

    return 1;
}

//查用户名表
int selectUser_MySql(MYSQL *conn, char *userName)
{
    //查询语句
    char query[128] = {0};
    printf("%s\n", userName);
    sprintf(query, "select username from user where username='%s';", userName);
    int ret = mysql_query(conn, query);
    if(ret != 0){
        printf("Query Error: %s\n", mysql_error(conn));
        return -1;
    }
    
    //获取查询的结果
    MYSQL_RES *result = mysql_store_result(conn);
    if(result == NULL){
        printf("Query Error2:%s\n", mysql_error(conn));
        return -1;
    }

    int rows = mysql_num_rows(result);

    mysql_free_result(result);
    return rows;
}

//查找用户信息
int selectPasswd_MySql(MYSQL *conn, char *userName, char *salt, char *passwd, char *path)
{
    //查询语句
    char query[128] = {0};
    sprintf(query, "select salt,cryptpasswd,pwd from user where username='%s';"
            , userName);
    int ret = mysql_query(conn, query);
    if(ret != 0){
        printf("Query Error: %s\n", mysql_error(conn));
        return -1;
    }

    //获取查询的结果
    MYSQL_RES *result = mysql_store_result(conn);
    if(result == NULL){
        printf("Query Error2:%s\n", mysql_error(conn));
        return -1;
    }
    
    //打印结果
    MYSQL_ROW row;
    int cols = mysql_num_fields(result);
    row = mysql_fetch_row(result);
    strcpy(salt, row[0]);
    strcpy(passwd, row[1]);
    strcpy(path, row[2]);

    //关闭结果集
    mysql_free_result(result);
    return 0;
}
