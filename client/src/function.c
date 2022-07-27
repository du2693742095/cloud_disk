#include "client.h"
#include "instruction.h"

//出错返回-1，关连接返回0，成功大于0
int handleCmd(cmd_hdl_t *cmdBuff, int clientfd){
    int ret = 0;
    switch(cmdBuff->cmd){
    case CMD_TYPE_LS:
    case CMD_TYPE_LL:
    case CMD_TYPE_PWD: 
    case CMD_TYPE_CP:
    case CMD_TYPE_CD:
    case CMD_TYPE_MV:
    case CMD_TYPE_RM:
    case CMD_TYPE_MKDIR:
        ret = recvFeedback(clientfd);
        break;
    case CMD_TYPE_PUTS:
        ret = putsFile(clientfd, cmdBuff);
        break;
    case CMD_TYPE_GETS:
        ret = getsFile(clientfd, cmdBuff);
        break;                                   
    case CMD_TYPE_EXIT:
        ret = exitClient(clientfd);
    default:
        break;
    }
    return ret;
}

int recvFeedback(int client)
{

}

