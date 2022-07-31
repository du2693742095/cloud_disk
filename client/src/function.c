#include "../include/function.h"

#define BUFSIZE 8192

//出错返回-1，关连接返回0，成功大于0
int handleCmd(cmd_hdl_t *cmdBuff, int clientfd){
    int ret = 0;
    switch(cmdBuff->cmd){
    case CMD_TYPE_LS:
    case CMD_TYPE_LL:
    case CMD_TYPE_PWD: 
    case CMD_TYPE_CP:
    case CMD_TYPE_CD:
    case CMD_TYPE_MV:
    case CMD_TYPE_RM:
    case CMD_TYPE_MKDIR:
        ret = recvFeedBack(clientfd);
        break;
    case CMD_TYPE_PUTS:
        ret = putsFile(clientfd, cmdBuff);
        break;
    case CMD_TYPE_GETS:
        ret = getsFile(clientfd, cmdBuff);
        break;                                   
    case CMD_TYPE_EXIT:
        ret = exitClient(clientfd);
    default:
        break;
    }
    return ret;
}

int recvFeedBack(int clientfd)
{
    char buf[BUFSIZE] = {0};
    int ret = recv(clientfd, buf, sizeof(buf), 0);
    if(0 ==ret){
        printf("server crush!\n");
        close(clientfd);
        exit(1);
    }
    else if(ret < 0){
        perror("recv");
        close(clientfd);
        exit(1);
    }
    else if(ret > 0){
        puts(buf);
    }
    return ret;
}

/*===================传输文件函数======================*/
// 获取路径下的文件名
void getFileNameByPath(char *path, int pathLen, char *fileName)
{
    int i = pathLen;
    while(i >= 0 && path[--i] != '/')
        ;
    memcpy(fileName, path + i + 1, pathLen - i);
}

//格式为：puts 目标文件 目的地址
int putsFile_Uncopy(int peerfd,cmd_hdl_t *cmdBuff)
{
    char *srcPath = cmdBuff->args[0].arg;
    int fd = open(srcPath, O_RDWR);
    if(fd == -1){
        size_t error = -1;
        int ret = send(peerfd, &error, sizeof(size_t), 0);
        return ret;
    }
    
    //用小火车发送目的地址
   // train_t fileName;
   // fileName.lenth = strlen(dirPath);
   // strcpy(fileName.buff, dirPath);
   // int ret = send(peerfd, &fileName, 
   //                sizeof(ssize_t) + fileName.lenth, MSG_WAITALL);
   // SEND_RECV_CHECK(ret, "send in transferFile_Uncopy");

    //获取文件信息长度并发送
    struct stat fileStat;
    fstat(fd, &fileStat);
    ssize_t fileLen = fileStat.st_size;
    int ret = send(peerfd, &fileLen, sizeof(fileLen), MSG_WAITALL);
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

int putsFile(int peerfd,cmd_hdl_t *cmdBuff)
{
    char *srcPath = cmdBuff->args[0].arg;
    //char *dirPath = cmdBuff->args[1].arg;
    int fd = open(srcPath, O_RDWR);
    if(fd == -1){
        size_t error = -1;
        int ret = send(peerfd, &error, sizeof(size_t), 0);
        return ret;
    }

    //用小火车发送文件名
    //train_t fileNameTrain;
    //strcpy(fileNameTrain.buff, dirPath);
    //fileNameTrain.lenth = strlen(dirPath);
    //int ret = send(peerfd, &fileNameTrain, sizeof(ssize_t)
    //               + fileNameTrain.lenth, MSG_WAITALL);
    //SEND_RECV_CHECK(ret, "send in transferFile");

    //获取文件信息长度，并发送
    struct stat fileStat;
    fstat(fd, &fileStat);
    ssize_t fileLen = fileStat.st_size;
    int ret = send(peerfd, &fileLen, sizeof(fileLen), MSG_WAITALL);
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

//命令为: get 源文件 目的地
int getFile_Uncopy(int peerfd, cmd_hdl_t *cmdBuff)
{
    //接收传输文件名
    //char name[100] = {0};
    //ssize_t nameSize;
    //int ret = recv(peerfd, &nameSize, sizeof(nameSize), 0);
    //ret = recv(peerfd, &name, nameSize, 0);
    //printf(" >>file name is %s\n", name);

    //测试 下载的名字是原名字
    char *name = (char *)cmdBuff->args[0].arg;
    int fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0644);

    //获取文件信息长度并构建文
    ssize_t fileLen;
    int ret = recv(peerfd, &fileLen, sizeof(fileLen), 0);
    if(fileLen == -1){
        printf("Error arguments.\n");
        return 1;
    }
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
    return ret;
}


//发文件, 格式为：get 目的地址 源地址
int getsFile(int peerfd, cmd_hdl_t *cmdBuff)
{
    //接收传输文件名
    //char name[100] = {0};
    //ssize_t nameSize;
    //int ret = recv(peerfd, &nameSize, sizeof(nameSize), 0);
    //ret = recv(peerfd, &name, nameSize, 0);
    //printf(" >>file name is %s\n", name);

    char *name = cmdBuff->args[1].arg;
    int fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0644);
    ERROR_CHECK(fd, -1, NULL);
    
    //获取文件信息长度并构建文件
    ssize_t fileLen;
    int ret = recv(peerfd, &fileLen, sizeof(fileLen), 0);
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
        //RECV_CHECK(ret);
        write(fd, buff, FILE_PER_LENTH);
        
        recvLen += msgLen;
        printf("has complete %5.2lf%%\r", 
               (double)recvLen / fileLen * 100);
        fflush(stdout);
    } while(recvLen < fileLen);

    printf("has complete 100.00%%\n");
    
    close(fd);
    close(peerfd);
    return ret;
}

int exitClient(int clientfd)
{
    close(clientfd);
    return 0;
}
