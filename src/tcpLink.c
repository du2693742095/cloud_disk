#include "../include/server.h"

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
    printf("+++++++++++++++++连接到服务器++++++++++++++++++++\n");
    printf("+++++++++++++++++ip: %s+++++++++++++++++++\n+", ip);
    printf("++++++++++++++++端口号：%d++++++++++++++++++\n", port);

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
