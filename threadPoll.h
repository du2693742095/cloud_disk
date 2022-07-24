#ifndef __THREAD_POLL_H__
#define __THREAD_POLL_H__

#include "server.h"
#include "taskqueue.h"

#define THREAD_ERROR_CHECK(ret, funcName)\
    do{\
        if(ret != 0){\
            fprintf(stderr, "%s: %s\n", funcName, strerror(ret));\
        }\
    }while(0)


//线程池
typedef struct{
    pthread_t *threads;         //线程池数组
    int threadNum;              //线程池个数
    pTaskQueue_t queue;         //任务队列
} threadPoll_t, *pThreadPoll;



#endif
