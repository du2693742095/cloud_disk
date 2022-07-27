#include "../include/client.h"

//初始化tcp连接，返回listenfd
int tcpInit(const char *ip, const int port)
{
    // socket.
    int clientfd = socket(AF_INET, SOCK_STREAM, 0); 
    ERROR_CHECK(clientfd, -1, NULL);

    // accept.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;

    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int ret = connect(clientfd, (struct sockaddr *)&addr, sizeof(addr));
    ERROR_CHECK(ret, -1, "connect");
    printf("+++++++++++++++++连接到服务器++++++++++++++++++++\n");
    printf("+++++++++++++++++ip: %s+++++++++++++++++++\n+", ip);
    printf("++++++++++++++++端口号：%d++++++++++++++++++\n", port);
    
    return clientfd;
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
