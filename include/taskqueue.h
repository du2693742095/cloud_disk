#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__

#include "server.h"

#define CMD_LENTH 20

typedef struct qNode_s{
    int peerfd;
    struct qNode_s *next;
    char cmd[CMD_LENTH];
    //用户名
    //char userName[];
    //用户路径
    //char userPath[];
}qNode_t;

typedef struct{
    qNode_t *front;
    qNode_t *rear;
    int size;
    int maxSize;
    //int flag;     //退出标志位
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} taskQueue_t, *pTaskQueue_t;

//如果send返回值为0链接关闭，则打印记录
//之后可以加一个handle_Func操作，链接关闭就执行
#define SEND_CHECK(ret, msg) do { \
    if(0 == ret){ \
        fprintf(stderr, "%s: connection is closed.\n", msg); retrun -1} \
    else if(-1 == ret){ \
        perror(msg); return -1; \
    } \
}while(0)

void queueInit(pTaskQueue_t queue, const int maxQueueSize); //队列初始化
void queueDestroy(pTaskQueue_t queue); //销毁队列
bool queueIsEmpty(const pTaskQueue_t queue); //判空
bool queueIsFull(const pTaskQueue_t queue); //判满
int queueSize(const pTaskQueue_t queue); //获取大小
void taskEnqueue(pTaskQueue_t queue, const int peerfd, const char *cmd); //入队
int taskDequeue(pTaskQueue_t queue, char *cmd); //出队

#endif
