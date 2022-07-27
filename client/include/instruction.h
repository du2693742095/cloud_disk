#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "client.h"

#define MAXCMDLEN 256

//指令集
typedef enum CMD{
    CMD_TYPE_OTHERS = -1,
    CMD_TYPE_EXIT,
    CMD_TYPE_CD,
    CMD_TYPE_MV,
    CMD_TYPE_CP,
    CMD_TYPE_LS,
    CMD_TYPE_LL,
    CMD_TYPE_PWD,
    CMD_TYPE_PUTS,
    CMD_TYPE_GETS,
    CMD_TYPE_RM,
    CMD_TYPE_MKDIR

} cmdType_t;

//参数数组
//1.参数长度；2.参数
typedef struct {
    int argLen;
    char arg[128];
}argument_t;

//指令传输小火车
//1.int 指令的长度，2.int 处理好的指令代号，3.int 参数个数；4.参数数组
//指令存储在cmd_hdl_t中（参考msghdl）
typedef struct {
    cmdType_t cmd;      //指令代码
    int argSize;        //指令参数的个数
    argument_t args[2]; //指令参数数组，最多两个参数
}cmd_hdl_t;

//发送，转私有协议，传出一个cmdType类型的指针，需要自己free！
//用完cmdType以后记得free！！！！！！
cmd_hdl_t *analyzeCmd(char *cmd, size_t cmdSize);
//处理指令
int handleCmd(cmd_hdl_t *cmdBuff, int peerfd);


#endif
