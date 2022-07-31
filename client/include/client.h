#ifndef __CLIENT_H__
#define __CLIENT_H__

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>

#include <dirent.h> //directorys
#include <pwd.h>    //getpwuid
#include <errno.h>  //errno

#include <sys/types.h>
#include <sys/stat.h>   //stat
#include <fcntl.h>      //unbuffered I/O
#include <sys/time.h>   //select
#include <unistd.h>     //nubuffered I/O
#include <sys/epoll.h>
#include <sys/mman.h>   //mmap
#include <sys/wait.h>   //wait
#include <signal.h>     //signal, sigaction
#include <pthread.h>    // multi_thread
#include <arpa/inet.h>  //port
#include <sys/socket.h> //socket.
#include <netinet/in.h> //inet_aton.
#include <netdb.h>      //gethostbyname
#include <sys/uio.h>    //rendv & writev

#define MSGLEN 1024
#define FILE_PER_LENTH 1024


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

//对send和recv发送的检查
#define SEND_RECV_CHECK(ret, msg)\
    if(ret == 0){\
        printf("connection is closed.\n");\
        exit(0);\
    }else if(ret == -1){\
        perror(msg);\
        exit(0);\
    }

//小火车
typedef struct {
    ssize_t lenth;
    char buff[FILE_PER_LENTH];
} train_t;


/*_____________________configInit.c___________________*/
//读取配置文件
int configInit(const char *path, char *ip, int *port);

#endif
