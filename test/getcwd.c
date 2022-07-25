#include <stdio.h>
#include <unistd.h>

int main()
{
    char *a = getcwd(NULL, 0);
    puts(a);
    return 0;
}

