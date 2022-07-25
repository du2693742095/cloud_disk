#include <stdio.h>
#include <string.h>

int main()
{
    char a[3];
    a[0] = '-';
    a[1] = 'a';
    a[2] = 'b';

    char buff[10];
    for(int i = 0; i < 3; i++){
        sprintf(buff, "%s-", buff);
    }
    puts(buff);
    return 0;
}

