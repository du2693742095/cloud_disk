#include "../include/client.h"
#include "../include/tcpLink.h"
#include "../include/instruction.h"

int main(int argc, char *argv[])
{
    ARG_CHECK(argc, 2);
    //初始化参数config
    char ip[16] = {0};
    int port = 0;
    int ret = configInit(argv[1], ip, &port);
    ARG_CHECK(ret, 2);
    
    
    int clientfd = tcpInit(ip, port);

    while(true){
        char msgBuff[128] = {0};
        //从键盘读取CMD命令
        ret = read(STDIN_FILENO, msgBuff, sizeof(msgBuff));

        //将指令转化为私有协议
        cmd_hdl_t *cmdBuff = analyzeCmd(msgBuff, strlen(msgBuff));
        if(cmdBuff == NULL) {
            printf("Invalid Instructions;\n");
            continue;
        }
        //向客户端发送指令
        ret = send(clientfd, cmdBuff, sizeof(cmd_hdl_t), 0);
        SEND_RECV_CHECK(ret, "send");

        //处理指令（接受服务器的信号或者是文件）
        ret = handleCmd(cmdBuff, clientfd);
        //用完指令以后将其free
        free(cmdBuff);
        if(ret == 0){
            puts("connection is closed.");
            close(clientfd);
            break;
        }else if(ret == -1){
            puts("error.");
            close(clientfd);
            break;
        }
    }
    puts("client is closed.\n");
    return 0;
}



