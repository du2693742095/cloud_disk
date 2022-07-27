#ifndef __TCPLINK_H__
#define __TCPLINK_H__

#include "client.h"

// 创建listenfd，返回listenfd的值
int tcpInit(const char *ip, const int port );

//修改监听事件模式函数
int epollAddReadEvent(const int fd, int epollfd);// 在epollfd中创建读操作监听
int epollAddReadWriteEvent(const int fd, int epollfd);//创建读写监听
void epollSetWriteEvent(const int fd, int epollfd);//设置写事件监听（修改）
void epollCleanWriteEvent(const int fd, int epollfd);//清除写事件监听（修改）


#endif
