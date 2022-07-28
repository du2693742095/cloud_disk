#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include "server.h"
#include "instruction.h"
#include <grp.h>

#define FUNCTION_ERROR_CHECK(ret, dir, msg, peerfd)\
    if (dir == ret){\
        char info[64] = {0};\
        sprintf(info, "%s: %s", msg, strerror(errno));\
        int a = send(peerfd, info, strlen(info) - 1, 0);\
        return a;\
    }

/*返回值int大于零表示执行成功，
 * 等于零表示连接关闭，
 * 小于零表示出错*/
int cdFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int mvFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int cpFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int lsFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int llFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int pwdFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int putsFile_Uncopy(const cmd_hdl_t *cmdBuff, int peerfd);
int putsFile(const cmd_hdl_t *cmdBuff, int peerfd);
int getsFile_Uncopy(const cmd_hdl_t *cmdBuff, int peerfd);
int getsFile(const cmd_hdl_t *cmdBuff, int peerfd);
int rmFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int mkdirFunc(const cmd_hdl_t *cmdBuff, int peerfd);
int errorCmdFunc(int peerfd);

#endif

