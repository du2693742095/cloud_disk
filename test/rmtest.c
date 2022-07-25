#include "function.h"

int main()
{
    char *path = "test";
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
        closedir(dp);
    }
    free(fstat);

    return 0;
}

