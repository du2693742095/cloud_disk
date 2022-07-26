#include "server.h"
#include "threadPoll.h"
#include "tcpLink.h"
#include "taskqueue.h"

int main(int argc, char *argv[])
{
    ARG_CHECK(argc, 2);
    //初始化服务器参数，并校验读到的参数是否为5个，如果不是就退出
    char ip[16] = {0};
    int port = 0, connectSize = 0, threadNum = 0, taskQueueSize = 0;
    int ret = configInit(argv[1], ip, &port, &connectSize, &threadNum, &taskQueueSize);
    ARG_CHECK(ret, 5);
    
    //创建监听和epoll对象
    int listenfd = createListenfd(ip, port);
    int epollfd = epoll_create1(0);
    ERROR_CHECK(epollfd, -1, "epoll_create.");
    epollAddReadEvent(listenfd, epollfd);
    //信号操作
    signal(SIGPIPE, SIG_IGN);//忽略管道中断
    //创建线程池
    threadPoll_t *pThreadPoll = (threadPoll_t *)
        malloc(sizeof(threadPoll_t));
    threadPoll_Init(pThreadPoll, threadNum, taskQueueSize);
    threadPoll_Start(pThreadPoll);
    //epoll监听开始
    struct epoll_event *pevList = (struct epoll_event *)
        calloc(connectSize, sizeof(struct epoll_event));
    int nready = 0;
    while(true){
        //暂时不设置超时
        do{
            nready = epoll_wait(epollfd, pevList, connectSize, 5000);
        }while(-1 == nready && errno == EINTR);

        if(0 == nready){
//日志：超时
            continue;
        }else if(-1 == nready){
            perror("epool_wait");
            return 1;
        }else{
            for(int i = 0; i < nready; ++i){
//优化点：获取对面的ip
                int peerfd = accept(listenfd, NULL, NULL);
                if(peerfd < 0){
//日志
                    perror("accept");
                }else{
                    queueInit(pThreadPoll->queue, taskQueueSize);
                    taskEnqueue(pThreadPoll->queue, peerfd);
                }
            }
        }
    }

    return 0;
}

