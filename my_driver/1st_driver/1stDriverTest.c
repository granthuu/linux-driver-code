
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>



int main(int argc, char * argv[])
{
    int fd = 0;
    char value = 0;
    
    fd = open("/dev/led", O_RDWR);
    if(fd <0)
    {
        printf("can't open file \n");
    }

    
    if(argc != 2)
    {
        printf("Usage: \n");
        printf("%s on|off\n", argv[0]);
        return 0;
    }

    if(strcmp(argv[1], "on") == 0)
        value = 1;
    else 
        value = 0;

    write(fd, &value, sizeof(value));
    
    return 0;
}

