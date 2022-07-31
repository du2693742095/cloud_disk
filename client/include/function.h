#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include "client.h"
#include "instruction.h"




int handleCmd(cmd_hdl_t *cmdBuff, int clientfd);
int recvFeedBack(int clientfd);
int putsFile_Uncopy(int clientfd, cmd_hdl_t *cmdBuff);
int putsFile(int clientfd, cmd_hdl_t *cmdBuff);
int getsFile_Uncopy(int clientfd, cmd_hdl_t *cmdBuff);
int getsFile(int clientfd, cmd_hdl_t *cmdBuff);
int exitClient(int clientfd);


#endif
