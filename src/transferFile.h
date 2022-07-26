#ifndef __TRANSFERFILE_H__
#define __TRANSFERFILE_H__

#include "server.h"

#define FILE_PER_LENTH 1024

//小火车
typedef struct {
    ssize_t lenth;
    char buff[FILE_PER_LENTH];
} train_t;

//定义send和recv返回值为0或者为-1时发生错误，直接返回-1
#define SEND_RECV_CHECK(ret, msg)\
    if(0 == ret){\
        fprintf(stderr, "%s: %s", msg, strerror(ret));\
        return -1;\
    }else if(ret < 0){\
        perror(msg);\
        return -1;\
    }

//零拷贝发送
int transferFile_Uncopy(char *path, const int peerfd);
//普通发送
int transferFile(char *path, const int peerfd);

/*______________先默认接收到本地文件夹，path默认为'.'_____________*/
//零拷贝接收
int recvFile_Uncopy(char *path, const int peerfd);
//普通接收
int recvFile(char *path, const int peerfd);

#endif
