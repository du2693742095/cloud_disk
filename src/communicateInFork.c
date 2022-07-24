#include "server.h"

int sendFd(int pipeFd, const int fd, char flag)
{
    //设置iovec
    struct iovec iov;
    iov.iov_base = &flag;
    iov.iov_len = 1;

    //设置cmsghdr
    size_t cmsLen = CMSG_LEN(sizeof(int));
    struct cmsghdr cmsg;
    cmsg.cmsg_level = SOL_SOCKET;
    cmsg.cmsg_type = SCM_RIGHTS;
    cmsg.cmsg_len = cmsLen;
    int *pfd = (int *)CMSG_DATA(&cmsg);
    *pfd = fd;

    //设置msghdr
    struct msghdr msg;
    memset(&msg, 0, sizeof(struct msghdr));
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &cmsg;
    msg.msg_controllen = cmsLen;

    int ret = sendmsg(pipeFd, &msg, 0);
    ERROR_EQUAL_CHECK(ret, -1, "sendmsg");

    return ret;
}

int recvFd(int pipeFd, int *fd, char *flag){
    //设置iovec，直接将flag所在地址作为参数，之后recvmsg会直接覆盖
    struct iovec iov;
    iov.iov_base = flag;
    iov.iov_len = 1;

    //设置cmsghdr，其中的数据段为int型的fd
     struct cmsghdr cmsg;
     cmsg.cmsg_level = SOL_SOCKET;
     cmsg.cmsg_type = SCM_RIGHTS;
     cmsg.cmsg_len = CMSG_LEN(sizeof(int));

    //设置msghdr
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &cmsg;
    msg.msg_controllen = cmsg.cmsg_len;

    int ret = recvmsg(pipeFd, &msg, 0);
    ERROR_EQUAL_CHECK(ret, -1, "recvmsg");

    int *p = (int *)CMSG_DATA(&cmsg);
    *fd = *p;

    return ret;
}

