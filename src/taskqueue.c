#include "../include/taskqueue.h"
#include "../include/threadPoll.h"

void queueInit(pTaskQueue_t queue, const int maxQueueSize) //队列初始化
{
    queue->front = queue->rear = NULL;
    queue->size = 0;
    queue->maxSize = maxQueueSize;
    int ret = pthread_mutex_init(&queue->mutex, NULL);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_init in queueInit");
    ret = pthread_cond_init(&queue->cond, NULL);
    THREAD_ERROR_CHECK(ret, "pthread_cond_init in queueInit");
}

void queueDestroy(pTaskQueue_t queue) //销毁队列
{
    if(queue != NULL){
        pthread_mutex_destroy(&queue->mutex);
        pthread_cond_destroy(&queue->cond);
        free(queue);
    }
}

bool queueIsEmpty(const pTaskQueue_t queue) //判空
{
    return queue->size <= 0;
}

bool queueIsFull(const pTaskQueue_t queue) //判满
{
    return queue->size >= queue->maxSize;
}

int queueSize(const pTaskQueue_t queue) //获取大小
{
    return queue->size;
}

void taskEnqueue(pTaskQueue_t queue, const int peerfd) //入队
{
    //第一步就要上锁
    int ret = pthread_mutex_lock(&queue->mutex);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_lock in taskEnqueue");
        
    //判满，队列不满就入队
    if(!queueIsFull(queue)){
        qNode_t *qNode = (qNode_t *)calloc(1, sizeof(qNode_t));
        //后期可以看要不把所有的申请空间函数加一个判错，写入日志
        qNode->peerfd = peerfd;
        qNode->next = NULL;

        //如果是队列为空，那就需要把头尾指针都指向这个元素
        //若非空，就只用加入表中，并把尾指针加一
        if(!queueIsEmpty(queue)){
            queue->rear->next = qNode;
            queue->rear = qNode;
        }else{
            queue->front = queue->rear = qNode;
        }
        ++queue->size;
    }

    ret = pthread_mutex_unlock(&queue->mutex);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_unlock in taskEnqueue");

    //通知环境变量可以消费了
    pthread_cond_signal(&queue->cond);

}

//出队，返回存储的peerfd，若出错，则返回-1
int taskDequeue(pTaskQueue_t queue)
{
    int ret = pthread_mutex_lock(&queue->mutex);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_lock in taskDequeue");

    //非空就出队，空就等待结点唤醒（就在while循环中）
    while(queueIsEmpty(queue)){
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    int retFd = -1;
    qNode_t *temp = queue->front;
    retFd = queue->front->peerfd;
    if(queueSize(queue)){
        queue->front = queue->front->next;
    }else{
        queue->front = queue->rear = NULL;
    }
    free(temp);

    ret = pthread_mutex_unlock(&queue->mutex);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_unlock in taskDequeue");

    return retFd;
}

