#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>


MYSQL * init_MySql(void);
void destory_MySql(MYSQL *conn);
int addUser_MySql(MYSQL *conn, char *userName, char *salt, char *crypass);
int selectUser_MySql(MYSQL *conn, char *userName);
int selectPasswd_MySql(MYSQL *conn, char *userName,
                       char *salt, char *passwd, char *path);
