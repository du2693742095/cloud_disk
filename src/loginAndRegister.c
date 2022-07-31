#include "../include/mysql.h"
#include "../include/server.h"

#define PASSWD_MAXLEN 20

//生成盐值
void saltCreate(char *salt)
{
    salt[0] = salt[8] = '$';
    int flag = 0;
    srand(time(NULL));
    //盐值的加密部分为6位，一共8位
    for(int i = 0; i < 6; ++i){
        flag = rand()%3;
        switch(flag){
        case 0: salt[i+1] = rand() % 26 + 'a'; break;
        case 1: salt[i+1] = rand() % 26 + 'A'; break;
        case 2: salt[i+1] = rand() % 10 + '0'; break;
        }
    }
    printf("salt is %s\n", salt);
}


//注册
int registerInServer(MYSQL *conn, int peerfd)
{
    int ret = 0;
    while(true){
        //接收用户名
        size_t nameLenth = 0;
        ret = recv(peerfd, &nameLenth, sizeof(nameLenth), 0);
        SEND_RECV_CHECK(ret, "register");
        char name[64] = {0};
        ret = recv(peerfd, name, nameLenth, 0);
        SEND_RECV_CHECK(ret, "register");
        //查重，若返回0则说明重复
        if(selectUser_MySql(conn, name) == -1){
            char error = '\0';
            ret = send(peerfd, &error, sizeof(char), 0);
            SEND_RECV_CHECK(ret, "register");
            continue;
            //没重复，那就生成盐值，接收密文
        }else{
            //生成并发送盐值
            char salt[8] = {0};
            saltCreate(salt);
            ret = send(peerfd, salt, strlen(salt), 0);
            SEND_RECV_CHECK(ret, "register");

            //获取密文，与自己加密后的对比
            char cryptPasswd[PASSWD_MAXLEN] = {0};
            ret = recv(peerfd, cryptPasswd, sizeof(cryptPasswd), 0);
            SEND_RECV_CHECK(ret, "register");

            addUser_MySql(conn, name, salt, cryptPasswd);
            break;
        }
    }
    return 1;
}

//登录
//int logIn()
