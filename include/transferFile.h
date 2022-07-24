#ifndef __TRANSFERFILE_H__
#define __TRANSFERFILE_H__

#include "server.h"

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
