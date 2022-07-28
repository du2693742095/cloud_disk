#include "../include/server.h"

//读取配置文件中的key
char *getKey(const char *line, char *key)
{
    int i = 0;
    while(line[i] != '='){
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
    while(line[i++] != '=')
        ;
    //处理一下linux和其它平台（mac等）行后缀不一样的bug
    while(line[i] != '\n' && line[i] != '\0'){
        value[j++] = line[i++];
    }
    value[j] = '\0';
    return value;
}

//读取配置文件
int configInit(const char *path, char *ip, int *port, int *connectSize,
                int *threadNum, int *tastQueueSize)
{
    FILE *file = fopen(path, "r");
    ERROR_CHECK(file, NULL, "open in configInit");
    
    char buff[64];
    int count = 0;
    while(1){
        memset(buff, 0, sizeof(buff));
        //循环读入一行数据，取出有效数据
        char *ret = fgets(buff, sizeof(buff), file);
        //设置退出条件
        if(ret == NULL) break;

        char key[16] = {0};
        char value[32] = {0};
        getKey(buff, key);
        getValue(buff, value);

        //判断key是什么数据，然后把后面的数据复制到对应的容器中
        if(!strcmp(key, "ip")){
            memcpy(ip, value, strlen(value) + 1);
            count++;
            //printf("ip = %s\n", ip);
        }
        else if(!strcmp(key, "port")){
            *port = atoi(value);
            count++;
            //printf("port= %d\n", *port);
        }else if(!strcmp(key, "connectSize")){
            *connectSize = atoi(value);
            count++;
        }
        else if(!strcmp(key, "thread_num")){
            *threadNum = atoi(value);
            count++;
            //printf("ip = %d\n", *threadNum);
        }
        else if(!strcmp(key, "tastQueue_size")){
            *tastQueueSize = atoi(value);
            count++;
            //printf("ip = %d\n", *tastQueueSize);
        }
    }
    return count;
}
