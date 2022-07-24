#include "server.h"

//读取配置文件中的key
char *getKey(const char *line, char *key)
{
    int i = 0;
    while(line[i] != ':'){
        key[i] = line[i];
        i++;
    }
    key[i] = '\0';
    return key;
}

//读取配置文件中的value
char *getValue(const char *line, char *value)
{
    int i = 0;
    int j = 0;
    while(line[i++] != ':')
        ;
    printf("i = %d\nline = %s\n",i , line + i);
    while(line[i] != '\0'){
        value[j++] = line[i++];
    }
    value[j] = '\0';
    return value;
}

//读取配置文件
void configInit(const char *path, char *ip, int *port,
                int *threadNum, int *tastQueueSize)
{
    FILE *file = fopen(path, "r");
    ERROR_CHECK(file, NULL, "open in configInit");
    
    char buff[64];
    while(1){
        memset(buff, 0, sizeof(buff));
        //循环读入一行数据，取出有效数据
        char *ret = fgets(buff, sizeof(buff), file);
        printf("%s",buff);
        //设置退出条件
        if(ret == NULL) break;

        char key[16] = {0};
        char value[32] = {0};
        getKey(buff, key);
        printf("key = %s\n", key);
        getValue(buff, value);
        printf("valuie = %s\n", value);

        //判断key是什么数据，然后把后面的数据复制到对应的容器中
        if(!strcmp(key, "ip")){
            memcpy(ip, value, strlen(value));
            printf("ip = %s\n", ip);
        }
        else if(!strcmp(key, "port")){
            *port = atoi(value);
            printf("port= %d\n", *port);
        }
        else if(!strcmp(key, "thread_num")){
            *threadNum = atoi(value);
            printf("ip = %d\n", *threadNum);
        }
        else if(!strcmp(key, "tastQueue_size")){
            *tastQueueSize = atoi(value);
            printf("ip = %d\n", *tastQueueSize);
        }
    }
}

//建立listenfd列表
int createListenfd(const char *ip, const int port) {
    // create and init socket.
    int listenfd = socket(AF_INET, SOCK_STREAM, 0); 
    ERROR_CHECK(listenfd, -1, "socket");

    // set socket substitute to ignore TIME_WAIT（设置网络地址可重用（SO_REUSEADDR）
    int optval = 1;
    int ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    ERROR_CHECK(ret, -1, "getsockopt");

    // bind.
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    ret = bind(listenfd, (struct sockaddr *)&addr, sizeof(addr));
    ERROR_CHECK(ret, -1, "bind");

    // listen client. max wait_list is 100.
    ret = listen(listenfd, 100);
    ERROR_CHECK(ret, -1, "listen");
    puts("listening socket succeed.");

    return listenfd;
}

//添加读事件
int epollAddReadEvent(const int fd, int epollfd){
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    ERROR_CHECK(ret, -1, "epoll_ctl");
    return ret;
}

//添加读写事件
int epollAddReadWriteEvent(const int fd, int epollfd){
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = fd;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    ERROR_CHECK(ret, -1, "epoll_ctl");
    return ret;
}

//设置写监听
void epollSetWriteEvent(const int fd, int epollfd){
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = fd;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    ERROR_CHECK(ret, -1, "epoll_ctl");
}

//清除写监听
void epollCleanWriteEvent(const int fd, int epollfd) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    ERROR_CHECK(ret, -1, "epoll_ctl");

}

//销毁连接
void epollDelConnect(const int fd, int epollfd) {
    struct epoll_event ev;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
    close(fd);
}
