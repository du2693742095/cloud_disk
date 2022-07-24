#ifndef __SERVE_H__
#define __SERVE_H__

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>

#include <dirent.h> //directorys
#include <pwd.h>    //getpwuid

#include <sys/types.h>
#include <sys/stat.h>   //stat
#include <fcntl.h>      //unbuffered I/O
#include <sys/time.h>   //select
#include <sys/select.h> //select
#include <unistd.h>     //nubuffered I/O
#include <sys/mman.h>   //mmap
#include <sys/wait.h>   //wait
#include <signal.h>     //signal, sigaction
#include <pthread.h>    // multi_thread
#include <arpa/inet.h>  //port
#include <sys/socket.h> //socket.
#include <netinet/in.h> //inet_aton.
#include <netdb.h>      //gethostbyname
#include <sys/epoll.h>  //epoll
#include <sys/uio.h>    //rendv & writev

#define BUSY true
#define FREE false     //默认为空闲
#define EXIT_FLAG 1
#define CONTINUE_FLAG 0
#define MSGLEN 1024

//如果send返回值为0链接关闭，则打印记录
//之后可以加一个handle_Func操作，链接关闭就执行
#define SEND_CHECK(ret, msg) do { \
    if(0 == ret){ \
        fprintf(stderr, "%s: connection is closed.\n", msg);} \
    else if(-1 == ret){ \
        perror(msg); return -1; \
    } \
}while(0)

// 检查参数是否正确
#define ARG_CHECK(argc, standard)\
    if(argc != standard){ \
        puts("Illigal arguments.");\
        exit(1);\
    }

//检查返回值是否正确，注意只能用于出错就要退出程序的地方
#define ERROR_CHECK(ret, standard, outPut)\
    do{\
        if(ret == standard){\
            perror(outPut);\
            exit(1);\
        }\
    } while(0)

//小火车
typedef struct {
    ssize_t lenth;
    char buff[MSGLEN];
} train_t;

/*_____________________建立链接的函数epolltalk.c___________________*/
//读取配置文件
void configInit(const char *path, char *ip, int *port,
                int *threadNum, int *tastQueueSize);
// 创建listenfd
int createListenfd(const char *ip, const int port );

//修改监听事件模式函数
int epollAddReadEvent(const int fd, int epollfd);// 在epollfd中创建读操作监听
int epollAddReadWriteEvent(const int fd, int epollfd);//创建读写监听
void epollSetWriteEvent(const int fd, int epollfd);//设置写事件监听（修改）
void epollCleanWriteEvent(const int fd, int epollfd);//清除写事件监听（修改）

// 处理新建的连接
void epollDelConnect(const int fd, int epollfd);

#endif
