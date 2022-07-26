#include "server.h"
#include "instruction.h"
#include "function.h"


/* 从peerfd中接收传输的小火车，将私有协议转化为数据，
 * 并判断是否有误
 * 注意：recvCmd必须和handleCmd一起使用，不然无法free(cmdBuff)*/
cmd_hdl_t *recvCmd(int peerfd)
{
    //接收指令长度
    int cmdLenth = 0;
    int ret = recv(peerfd, &cmdLenth, sizeof(int), 0);

    //接收指令剩余部分
    char buff[MAXCMDLEN];
    ret = recv(peerfd, buff, cmdLenth, MSG_WAITALL);

    //拆分私有协议，分析出指令和参数
    //1.指令代码，2.参数个数，3.参数数组
    int pos = 0;
    cmd_hdl_t *cmdBuff = (cmd_hdl_t *)malloc(sizeof(cmd_hdl_t));
    memcpy(&cmdBuff->cmd, buff + pos, sizeof(int));//指令代码
    pos += sizeof(int);
    memcpy(&cmdBuff->argSize, buff + pos, sizeof(int));//参数长度
    pos += sizeof(int);
    //拆分参数
    int i = 0;
    while(i < cmdBuff->argSize){
        //获取指令长度;
        memcpy(&cmdBuff->args->argLen, buff + pos, sizeof(int));
        pos += sizeof(int);
        //复制参数
        memcpy(cmdBuff->args->arg, buff + pos, cmdBuff->args->argLen);
        pos += cmdBuff->args->argLen;
        i++;
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
        ret = putsFunc(cmdBuff, peerfd);
        break;
    case CMD_TYPE_GETS:
        ret = getsFunc(cmdBuff, peerfd);
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

