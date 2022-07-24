#include "server.h"

char *getKey(const char *, char *);
char *getValue(const char *, char *);
void configInit(const char *path, char *ip, int *port,
                int *threadNum, int *tastQueueSize);

int main(int argc, char *argv[])
{
   // char key[16];
   // char value[16];
   // getKey("ip:192.168.174.129", key);
   // getValue("ip:192.168.174.129", value);
   // printf("key = %s\nvalue = %s", key, value);

    char ip[16]  = {0};
    int port = 0;
    int threadNum = 0;
    int tastQueueSize = 0;
    configInit(argv[1], ip, &port, &threadNum, &tastQueueSize);
    printf("ip = %s\nport = %d\nthreadNum = %d\ntashQueueSize = %d\n",
           ip, port, threadNum, tastQueueSize);

    return 0;
}

