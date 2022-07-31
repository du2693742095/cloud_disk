#include "../include/client.h"

#define PASSWD_MAXLEN 20

void registerInClient(const int clientfd)
{
    while(true){
        printf("username is: ");
        fflush(stdout);
        //接收用户名并发送
        train_t name;
        bzero(&name, sizeof(name));
        read(STDIN_FILENO, name.buff, sizeof(name.buff)); //注意会读入\n，需要strlen-1
        name.lenth = strlen(name.buff) - 1;
        send(clientfd, name.buff, name.lenth + sizeof(size_t), 0);

        //接收返回值
        char salt[8] = {0};
        recv(clientfd, salt, sizeof(salt), 0);
        if(salt[0] != '$'){
            printf("\n%s has exist. please try again.\n", name.buff);
            continue;
        }else{        
            printf("\npasswd is: ");
            fflush(stdout);
            //如果正确，那就开始输入密码，并加密后传输
            char passwd[PASSWD_MAXLEN] = {0};
            read(STDIN_FILENO, passwd, sizeof(passwd));
            char *cryptPasswd = crypt(salt, passwd);
            send(clientfd, cryptPasswd, strlen(cryptPasswd), 0);

            printf("%s 创建成功\n", name.buff);
            break;

        }
    }
}
