#include "threadPoll.h"
#include "function.h"

//初始化线程池，顺便创建任务队列
void threadPoll_Init(pThreadPoll_t threadPoll, const int threadSize, const int tastQueueSize)
{
    memset(threadPoll, 0, sizeof(threadPoll_t));
    //建立线程池数组
    threadPoll->threads = (pthread_t *)calloc(threadSize, sizeof(pthread_t));
    threadPoll->threadNum = threadSize;
    queueInit(threadPoll->queue, tastQueueSize);
}

//销毁线程池
void threadPoll_Destroy(pThreadPoll_t threadPoll)
{
    free(threadPoll->threads);
    queueDestroy(threadPoll->queue);
}

//创建线程池
void threadPoll_Start(pThreadPoll_t threadPoll)
{
    for(int i = 0; i < threadPoll->threadNum; ++i){
        int ret = pthread_create(&threadPoll->threads[i], NULL, 
                                 threadFunc, threadPoll);
        THREAD_ERROR_CHECK(ret, "pthread_create");
    }
}

void * threaddFunc(void *argv)
{
    pThreadPoll_t threadPoll = (threadPoll_t *)argv;
    while(true){
        char cmd[CMD_LENTH] = {0};
        int peerfd = taskDequeue(threadPoll->queue, cmd);
        //将命令转化为宏
        cmdType ins = analyzeCmd(cmd);
        switch(ins){
            case CMD_TYPE_CD:
                break;
            case CMD_TYPE_MV:
                break;
            case CMD_TYPE_CP:
                break;
            case CMD_TYPE_LS:
                break;
            case CMD_TYPE_LL:
                break;
            case CMD_TYPE_PWD:
                break;
            case CMD_TYPE_PUTS:
                break;
            case CMD_TYPE_GETS:
                break;
            case CMD_TYPE_RM:
                break;
            case CMD_TYPE_MKDIR:
                break;
            default:
                errorCmd_Func();
                break;
        }
    }

    pthread_exit(0);
}


