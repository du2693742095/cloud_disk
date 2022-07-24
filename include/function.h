#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include "server.h"
#include "instruction.h"

int cdFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int mvFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int cpFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int lsFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int llFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int pwdFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int putsFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int getsFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int rmFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int mkdirFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int errorCmdFunc(void);

#endif

