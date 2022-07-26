#include "transferFile.h"

// 获取路径下的文件名
void getFileNameByPath(char *path, int pathLen, char *fileName)
{
    int i = pathLen;
    while(i >= 0 && path[--i] != '/')
        ;
    memcpy(fileName, path + i + 1, pathLen - i);
}

//零拷贝技术
int transferFile_Uncopy(char *path, const int peerfd)
{
    int fd = open(path, O_RDWR);
    ERROR_CHECK(fd, -1, "open");
    
    //用小火车发送文件名
    train_t fileName;
    getFileNameByPath(path, strlen(path), fileName.buff);
    fileName.lenth = strlen(fileName.buff);
    int ret = send(peerfd, &fileName, 
                   sizeof(ssize_t) + fileName.lenth, MSG_WAITALL);
    SEND_RECV_CHECK(ret, "send in transferFile_Uncopy");

    //获取文件信息长度并发送
    struct stat fileStat;
    fstat(fd, &fileStat);
    ssize_t fileLen = fileStat.st_size;
    printf("    > file is %s, filesize is %ld.\n", fileName.buff, fileLen);
    ret = send(peerfd, &fileLen, sizeof(fileLen), MSG_WAITALL);
    SEND_RECV_CHECK(ret, "send in transferFile_Uncopy"); 

    //通过零拷贝的splice传输文件，一次传输FILE_PRE_LENTH个长度
    int pipefd[2];
    pipe(pipefd);
    ssize_t sendLen = 0;
    while(sendLen < fileLen){
        ssize_t msglen = splice(fd, NULL, pipefd[1], NULL, 
                                FILE_PER_LENTH, SPLICE_F_MORE);
        msglen = splice(pipefd[0], NULL, peerfd, NULL, msglen, SPLICE_F_MORE);
        //处理链接关闭的情况
        if(0 == msglen) {
            printf("Connection is closed.\n");
            break;
        } else if(-1 == msglen){
            perror("splice");
            break;
        }
        sendLen += msglen;
    }     
    return fileLen - sendLen;
}

//send实现，每次发送MSGLEN个
int transferFile(char *path, const int peerfd)
{
    int fd = open(path, O_RDWR);
    ERROR_CHECK(fd, -1, "open");

    //用小火车发送文件名
    train_t fileNameTrain;
    getFileNameByPath(path, strlen(path), fileNameTrain.buff);
    fileNameTrain.lenth = strlen(fileNameTrain.buff);
    int ret = send(peerfd, &fileNameTrain, sizeof(ssize_t)
                   + fileNameTrain.lenth, MSG_WAITALL);
    SEND_RECV_CHECK(ret, "send in transferFile");

    //获取文件信息长度，并发送
    struct stat fileStat;
    fstat(fd, &fileStat);
    ssize_t fileLen = fileStat.st_size;
    printf("    > file is %s, filesize is %ld.\n", fileNameTrain.buff, fileLen);
    ret = send(peerfd, &fileLen, sizeof(fileLen), MSG_WAITALL);
    SEND_RECV_CHECK(ret, "send in transferFile");

    //构建小火车，一次发送FILE_PER_LENTH个长度的数据
    train_t fileTrain;
    ssize_t sendLen = 0;
    while(sendLen < fileLen){
        memset(&fileTrain, 0, sizeof(fileTrain));
        //读入数据，放到小火车数据段
        int msglen = read(fd, fileTrain.buff, FILE_PER_LENTH);
        fileTrain.lenth = msglen;

        msglen = send(peerfd, &fileTrain, sizeof(ssize_t)
                      + fileTrain.lenth, MSG_WAITALL);
        SEND_RECV_CHECK(msglen, "send in transferFile");
        sendLen += msglen;
    }
    printf("send secceed.\n");
    return fileLen - sendLen;
}

//零拷贝接收
int recvFile_Uncopy(char *path, const int peerfd)
{
    //接收传输文件名
    char name[100] = {0};
    ssize_t nameSize;
    int ret = recv(peerfd, &nameSize, sizeof(nameSize), 0);
    ret = recv(peerfd, &name, nameSize, 0);
    printf(" >>file name is %s\n", name);

    int fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0644);
    ERROR_CHECK(fd, -1, NULL);
    
    //获取文件信息长度并构建文件
    ssize_t fileLen;
    ret = recv(peerfd, &fileLen, sizeof(fileLen), 0);
    printf(" >>file size is %ld\n", fileLen);
    ftruncate(fd, fileLen);

    //通过零拷贝的splice传输文件，一次传输MSGLEN个长度
    int pipefd[2];
    pipe(pipefd);
    ssize_t recvLen = 0;
    do {
        ssize_t msglen = splice(peerfd, NULL, pipefd[1], NULL, FILE_PER_LENTH, SPLICE_F_MORE);
        msglen = splice(pipefd[0], NULL, fd, NULL, msglen, SPLICE_F_MORE);
        //RECV_CHECK(msglen);

        recvLen += msglen;
        printf("has complete %5.2f%%\r", 
               (double)recvLen / fileLen * 100);
        fflush(stdout);
    } while(recvLen < fileLen);

    printf("has complete 100.00%%\n");
    close(fd);
    close(peerfd);
    return 0;
}

//send实现，每次发送MSGLEN个
int recvFile(char *path, const int peerfd)
{
    //接收传输文件名
    char name[100] = {0};
    ssize_t nameSize;
    int ret = recv(peerfd, &nameSize, sizeof(nameSize), 0);
    ret = recv(peerfd, &name, nameSize, 0);
    printf(" >>file name is %s\n", name);

    int fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0644);
    ERROR_CHECK(fd, -1, NULL);
    
    //获取文件信息长度并构建文件
    ssize_t fileLen;
    ret = recv(peerfd, &fileLen, sizeof(fileLen), 0);
    printf(" >>file size is %ld\n", fileLen);
    ftruncate(fd, fileLen);

    //构建小火车，一次发送MSGLEN个长度的数据
    ssize_t recvLen = 0;
    do {
        char buff[FILE_PER_LENTH];
        //读入数据，放到小火车数据段
        ssize_t msgLen;
        ret = recv(peerfd, &msgLen, sizeof(msgLen), MSG_WAITALL);
        ret = recv(peerfd, buff, msgLen, MSG_WAITALL);
        if(ret == 0){
            printf("connection is closed.\n");
            close(fd);
            return -1;
        }
        //RECV_CHECK(ret);
        write(fd, buff, FILE_PER_LENTH);
        
        recvLen += msgLen;
        printf("has complete %5.2lf%%\r", 
               (double)recvLen / fileLen * 100);
        fflush(stdout);
    } while(recvLen < fileLen);

    printf("has complete 100.00%%\n");
    
    close(fd);
    return 0;
}
