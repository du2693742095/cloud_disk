#include "server.h"

//mkdir and rm 
// mkdir，传入要创建的文件夹名char *，返回是否成功
// 默认创建的权限为644
int mkdir(const char *path)
{
    return mkdir(path, 0664);
}

// rm，删除文件，传入文件的地址，返回是否成功
int rm(const char *path)
{	
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
            char fileName[256] = {0};
            sprintf(fileName, "%s/%s", path, director->d_name);
            unlink(fileName);
        }
        rmdir(path);
    }
    return 0;
}
