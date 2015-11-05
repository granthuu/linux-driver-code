
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>



int main(int argc, char * argv[])
{
    int fd = 0;
    fd = open("/dev/led", O_RDWR);

    return 0;
}

