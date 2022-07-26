#include "threadPoll.h"
#include "instruction.h"

void * threadFunc(void *argv);

//初始化线程池，顺便创建任务队列
void threadPoll_Init(pThreadPoll_t threadPoll, const int threadSize, const int tastQueueSize)
{
    //建立线程池数组
    threadPoll->threads = (pthread_t *)calloc(threadSize, sizeof(pthread_t));
    threadPoll->threadNum = threadSize;
    threadPoll->queue = (taskQueue_t *)malloc(sizeof(taskQueue_t));
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

void * threadFunc(void *argv)
{
    printf("%ld thread is working.\n", pthread_self());
    pThreadPoll_t threadPoll = (threadPoll_t *)argv;

    while(true){
        int peerfd = taskDequeue(threadPoll->queue);
        //接收命令（内含判错）
        int ret = 0;
        do {
            //接收函数
            cmd_hdl_t *cmdBuff = recvCmd(peerfd);
            //处理命令
            ret = handleCmd(cmdBuff, peerfd);
        }while(ret > 0);
        close(peerfd);
    }
    pthread_exit(0);
}


