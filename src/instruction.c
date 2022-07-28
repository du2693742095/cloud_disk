#include "../include/server.h"
#include "../include/instruction.h"
#include "../include/function.h"


/* 从peerfd中接收传输的小火车，将私有协议转化为数据，
 * 并判断是否有误
 * 注意：recvCmd必须和handleCmd一起使用，不然无法free(cmdBuff)*/
cmd_hdl_t *recvCmd(int peerfd)
{
    cmd_hdl_t *cmdBuff = (cmd_hdl_t *)malloc(sizeof(cmd_hdl_t));
    memset(cmdBuff, 0, sizeof(cmd_hdl_t));
    int ret = recv(peerfd, cmdBuff, sizeof(cmd_hdl_t), MSG_WAITALL);
    printf("%d\n",cmdBuff->cmd);
    //链接关闭
    if(ret <= 0){
        return NULL;
    }
    return cmdBuff;
}

//返回是否退出，小于零则表示连接断开或者客户端发来exit命令
int handleCmd(cmd_hdl_t *cmdBuff, int peerfd)
{
    int ret = 0;
    switch(cmdBuff->cmd){
    case CMD_TYPE_CD:
        ret = cdFunc(cmdBuff, peerfd);
        break;
    case CMD_TYPE_MV:
        ret = mvFunc(cmdBuff, peerfd);
        break;
    case CMD_TYPE_CP:
        ret = cpFunc(cmdBuff, peerfd);
        break;
    case CMD_TYPE_LS:
        ret = lsFunc(cmdBuff, peerfd);
        break;
    case CMD_TYPE_LL:
        ret = llFunc(cmdBuff, peerfd);
        break;
    case CMD_TYPE_PWD:
        ret = pwdFunc(cmdBuff, peerfd);
        break;
    case CMD_TYPE_PUTS:
        ret = putsFile(cmdBuff, peerfd);
        break;
    case CMD_TYPE_GETS:
        ret = getsFile(cmdBuff, peerfd);
        break;
    case CMD_TYPE_RM:
        ret = rmFunc(cmdBuff, peerfd);
        break;
    case CMD_TYPE_MKDIR:
        ret = mkdirFunc(cmdBuff, peerfd);
        break;
    case CMD_TYPE_EXIT:
        ret = -1;
        break;
    default:
        ret = errorCmdFunc(peerfd);
        break;
    }
    free(cmdBuff);
    return ret;
}

