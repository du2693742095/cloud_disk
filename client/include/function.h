#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include "client.h"
#include "instruction.h"

#define FILE_PER_LENTH 1024

//小火车
typedef struct {
    ssize_t lenth;
    char buff[FILE_PER_LENTH];
} train_t;


int handleCmd(cmd_hdl_t *cmdBuff, int clientfd);
int recvFeedBack(int clientfd);
int putsFile_Uncopy(int clientfd, cmd_hdl_t *cmdBuff);
int putsFile(int clientfd, cmd_hdl_t *cmdBuff);
int getsFile_Uncopy(int clientfd, cmd_hdl_t *cmdBuff);
int getsFile(int clientfd, cmd_hdl_t *cmdBuff);
int exitClient(int clientfd);


#endif
