#ifndef __SERVE_H__
#define __SERVE_H__

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


/*_____________________configInit.c___________________*/
//读取配置文件
int configInit(const char *path, char *ip, int *port, int *connectSize,
                int *threadNum, int *tastQueueSize);


#endif
