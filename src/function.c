#include "../include/server.h"
#include "../include/function.h"
#include "../include/transferFile.h"

//将当前目录切换
int cdFunc(const cmd_hdl_t *cmdBuff, int peerfd)
{
    char *path = NULL;
    if(cmdBuff->argSize == 0){
        path = "/home/";
    }
    else if(cmdBuff->argSize == 1){
        path = (char *)cmdBuff->args[0].arg;
    }else{
        //error
        send(peerfd, "error arguments.", 17, 0);
        return  -1;
    }

    int ret = chdir(path);
    FUNCTION_ERROR_CHECK(ret, -1, "chdir", peerfd);
    char info[] = "cd is succeed.\n";

    /* 因为chdir的ret已经用FUNCTION_ERROR_CHECK检查过了，里面
     * 有return函数，所以继续到最后一步的ret肯定是不为-1的，
     * 故可以直接赋值，交给主函数去判断返回值是否小于零*/
    ret = send(peerfd, info, sizeof(info), 0);
    return ret;
}

//将文件或文件夹重命名或者移动，类似mv效果
int mvFunc(const cmd_hdl_t *cmdBuff, int peerfd)
{
    if(cmdBuff->argSize != 2){
        send(peerfd, "missing file operand.", 22, 0);
        return -1;
    }

    const char *oldname = cmdBuff->args[0].arg; 
    const char *newname = cmdBuff->args[1].arg;
    int ret = rename(oldname, newname);
    FUNCTION_ERROR_CHECK(ret, -1, "mv", peerfd);
    char info[] = "mv is succeed.\n";
    ret = send(peerfd, info, sizeof(info), 0);
    return ret;
}

/*————————————————文件复制函数————————————————*/
int cpFile(const char *srcPath, const char *dirPath, int peerfd)
{
    int readFd = open(srcPath, O_RDONLY);
    FUNCTION_ERROR_CHECK(readFd, -1, "open srcPath", peerfd);
    int writeFd = open(dirPath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    FUNCTION_ERROR_CHECK(writeFd, -1, "open dirPath", peerfd);

    char buff[1024] = {0};
    int len;
    while((len = read(readFd, buff, sizeof(buff))) != 0){
        memset(buff, 0, sizeof(buff));
        write(writeFd, buff, len);
    }
    close(readFd);
    close(writeFd);
    return 0;
}

int cpDir(const char *srcPath, const char *dirPath, int peerfd)
{
    DIR *dp = opendir(srcPath);
    FUNCTION_ERROR_CHECK(dp, NULL, "opendir in srcPath", peerfd);

    struct dirent *director;
    while((director = readdir(dp)) != NULL){
        char *name = director->d_name;
        // ignore . & ..
        if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0){
            continue;
        }
        char real_dpath[128];
        char real_spath[128];
        sprintf(real_dpath, "%s/%s", dirPath, name);
        sprintf(real_spath, "%s/%s", srcPath, name);
        if(director->d_type == DT_DIR){
            cpDir(real_dpath, real_spath, peerfd);
        }
        else{
            cpFile(real_dpath, real_spath, peerfd);
        }
    }
    closedir(dp);
    return 0;
}

//实现文件复制或者目录递归复制
int cpFunc(const cmd_hdl_t *cmdBuff, int peerfd)
{
    //校验参数
    if(cmdBuff->argSize != 2){
        return -1;
    }

    const char *srcPath = cmdBuff->args[0].arg;
    const char *dirPath = cmdBuff->args[1].arg;

    struct stat *fstat = (struct stat *)calloc(1, sizeof(struct stat));
    stat(srcPath, fstat);

    //判断路径是否为文件夹
    if((fstat->st_mode & S_IFMT) != S_IFDIR){
        //如果是文件夹，那就递归复制
        cpFile(srcPath, dirPath, peerfd);
    }else{
        cpDir(srcPath, dirPath, peerfd);
    }

    char info[] = "cp is succeed.\n";
    int ret = send(peerfd, info, sizeof(info), 0);
    return ret;
}

/*---------------------目录操作---------------------------*/
//展示目录信息
int lsFunc(const cmd_hdl_t *cmdBuff, int peerfd)
{
    DIR* pdir;
    //参数校验
    char *currentPath;
    if(cmdBuff->argSize == 0){
        currentPath = ".";
    }else{
        currentPath = (char *)cmdBuff->args[0].arg;
    }

    pdir = opendir(currentPath);
    FUNCTION_ERROR_CHECK(pdir, NULL, "opendir", peerfd);

    //读目录项
    struct dirent* pdirent;
    char resultBuff[8192] = {0};
    while((pdirent = readdir(pdir)) != NULL)
    {
        //获取文件的完整路径
        char* name = pdirent->d_name;
        //忽略隐藏文件和隐藏文件夹
        if(name[0] == '.')  continue;
        //特殊处理第一个文件名
        if(resultBuff[0] != 0){
            sprintf(resultBuff, "%s\t%s", resultBuff, name);
        }else{
            strcpy(resultBuff, name);
        }
    }
    //关闭目录流
    closedir(pdir);

    int ret = send(peerfd, resultBuff, strlen(resultBuff), MSG_WAITALL);
    return ret;
}

//展示目录详细信息
int llFunc(const cmd_hdl_t *cmdBuff, int peerfd)
{
    //校验参数
    char *currentPath;
    if(cmdBuff->argSize == 0){
        currentPath = ".";
    }else{
        currentPath = (char *)cmdBuff->args[0].arg;
    }

    DIR *dp = opendir(currentPath);
    FUNCTION_ERROR_CHECK(dp, NULL, "opendir", peerfd);
    
    struct dirent *director;
    char resultBuff[8192] = {0};

    //循环读入目录，并输入数据
    while((director = readdir(dp)) != NULL){
        // 获取目录中的信息
        struct stat statbuf;
        int result_stat = stat(director->d_name, &statbuf);
        // 打印文件信息
        switch(statbuf.st_mode & S_IFMT) {
        case S_IFREG:   sprintf(resultBuff, "%s-", resultBuff);    break;
        case S_IFDIR:   sprintf(resultBuff, "%sd", resultBuff);    break;
        case S_IFBLK:   sprintf(resultBuff, "%sb", resultBuff);    break;
        case S_IFCHR:   sprintf(resultBuff, "%sc", resultBuff);    break;
        case S_IFIFO:   sprintf(resultBuff, "%sp", resultBuff);    break;
        case S_IFLNK:   sprintf(resultBuff, "%sl", resultBuff);    break;
        default:        sprintf(resultBuff, "%s?", resultBuff);    break;
        }
        // 打印权限信息
        for(int i = 0; i < 3; i++){
            mode_t mode = (statbuf.st_mode >> (3 * (2 - i))) & 0x7;
            // 1可执行，2写，3写执行，4只读，5读执行，6读写，7全部
            switch(mode){
            case 1:     sprintf(resultBuff, "%s--x", resultBuff);    break;
            case 2:     sprintf(resultBuff, "%s-w-", resultBuff);    break;
            case 3:     sprintf(resultBuff, "%s-wx", resultBuff);    break;
            case 4:     sprintf(resultBuff, "%sr--", resultBuff);    break;
            case 5:     sprintf(resultBuff, "%sr-x", resultBuff);    break;
            case 6:     sprintf(resultBuff, "%srw-", resultBuff);    break;
            case 7:     sprintf(resultBuff, "%srwx", resultBuff);    break;
            defualt:    sprintf(resultBuff, "%s???", resultBuff);    break;
            }
        }
        // 链接数, 拥有者和所属的组名, 大小
        struct passwd *username = getpwuid(statbuf.st_uid);
        struct group *groupname = getgrgid(statbuf.st_gid);
        sprintf(resultBuff, "%s%ld %s %s %6ld", resultBuff, statbuf.st_nlink,
                username->pw_name, groupname->gr_name, statbuf.st_size);
        //打印时间和文件名
        struct tm *time = localtime(&statbuf.st_atime);
        sprintf(resultBuff, "%s%2d月 %.2d %.2d:%.2d %s\n", resultBuff,
                time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min,
                director->d_name);
    }
    closedir(dp);
    int ret = send(peerfd, resultBuff, strlen(resultBuff), MSG_WAITALL);
    return ret;
}

//显示当前路径
int pwdFunc(const cmd_hdl_t *cmdBuff, int peerfd)
{
    if(cmdBuff->argSize != 0){
        send(peerfd, "error arguments.\n", 17, 0);
        return -1;
    }
    char *buff = getcwd(NULL, 0);
    int ret = send(peerfd, buff, strlen(buff), 0);
    return ret;
}

/*=============================收发文件===================*/
//格式为：puts 目标文件 目的地址
int putsFile_Uncopy(const cmd_hdl_t *cmdBuff, int peerfd)
{
    char *srcPath = (char *)cmdBuff->args[0].arg;
    //char *dirPath = cmdBuff->args[1].arg;
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

int putsFile(const cmd_hdl_t *cmdBuff, int peerfd)
{
    char *srcPath = (char *)cmdBuff->args[0].arg;
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
int getFile_Uncopy(const cmd_hdl_t *cmdBuff, int peerfd)
{
    //接收传输文件名
    //char name[100] = {0};
    //ssize_t nameSize;
    //int ret = recv(peerfd, &nameSize, sizeof(nameSize), 0);
    //ret = recv(peerfd, &name, nameSize, 0);
    //printf(" >>file name is %s\n", name);

    char *name = (char *)cmdBuff->args[1].arg;
    int fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0644);
    ERROR_CHECK(fd, -1, NULL);
    
    //获取文件信息长度并构建文件
    ssize_t fileLen;
    int ret = recv(peerfd, &fileLen, sizeof(fileLen), 0);
    if(fileLen == -1){
        printf("Error file path.\n");
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
        //printf("has complete %5.2f%%\r", 
        //       (double)recvLen / fileLen * 100);
        fflush(stdout);
    } while(recvLen < fileLen);

    //printf("has complete 100.00%%\n");
    close(fd);
    close(peerfd);
    return ret;
}


//发文件, 格式为：get 目的地址 源地址
int getsFile(const cmd_hdl_t *cmdBuff, int peerfd)
{
    //接收传输文件名
    //char name[100] = {0};
    //ssize_t nameSize;
    //int ret = recv(peerfd, &nameSize, sizeof(nameSize), 0);
    //ret = recv(peerfd, &name, nameSize, 0);
    //printf(" >>file name is %s\n", name);

    char *name = (char *)cmdBuff->args[1].arg;
    int fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0644);
    ERROR_CHECK(fd, -1, NULL);
    
    //获取文件信息长度并构建文件
    ssize_t fileLen;
    int ret = recv(peerfd, &fileLen, sizeof(fileLen), 0);
    //printf(" >>file size is %ld\n", fileLen);
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
        //printf("has complete %5.2lf%%\r", 
        //       (double)recvLen / fileLen * 100);
        fflush(stdout);
    } while(recvLen < fileLen);

    //printf("has complete 100.00%%\n");
    
    close(fd);
    close(peerfd);
    return ret;
}


/*===================删除文件======================*/
//删除文件或文件夹
int rmFunc(const cmd_hdl_t *cmdBuff, int peerfd)
{
    if(cmdBuff->argSize != 1){
        send(peerfd, "error arguments.\n", 17, 0);
        return -1;
    }
    char *path = (char *)cmdBuff->args[0].arg;
    struct stat *fstat = (struct stat *)calloc(1, sizeof(struct stat));
    stat(path, fstat);

    //判断路径是否为文件夹
    if((fstat->st_mode & S_IFMT) != S_IFDIR){
        unlink(path);
    }else{
        DIR *dp = opendir(path);
        ERROR_CHECK(dp, NULL, "opendir in rm");
        struct dirent *director;
        while((director = readdir(dp)) != NULL){
            char fileName[512] = {0};
            sprintf(fileName, "%s/%s", path, director->d_name);
            unlink(fileName);
        }
        rmdir(path);
        closedir(dp);
    }
    free(fstat);

    char info[] = "rm is succeed.\n";
    int ret = send(peerfd, info, sizeof(info), 0);
    return ret;
}

int mkdirFunc(const cmd_hdl_t *cmdBuff, int peerfd)
{
    if(cmdBuff->argSize != 1){
        send(peerfd, "error arguments.\n", 17, 0);
        return -1;
    }
    int ret = mkdir(cmdBuff->args[0].arg, 0664);
    return ret;
}

//错误指令
int errorCmdFunc(int peerfd)
{
    int ret = send(peerfd, "Invalid Commands.\n", 18, 0);
    return ret;
}
