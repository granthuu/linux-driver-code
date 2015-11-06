#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define TEST_IOCTRL_FUN

#ifdef TEST_IOCTRL_FUN

int main(int argc, char * argv[])
{
    int fd;
    int cmd, led_num;
    
    fd = open("/dev/Button", O_RDWR);
    if(fd < 0)
        printf("can't open file \n");

    if(argc < 3)
    {
        printf("Usage: \n");
        printf("%s on|off 0|1|2\n", argv[0]);
        return 0;
    }

    if(strcmp(argv[1], "on") == 0)
    {
        cmd = 1;
    }
    else
        cmd = 0;

    if(strcmp(argv[2], "0") == 0)
        led_num = 0;
    else if(strcmp(argv[2], "1") == 0)
        led_num = 1;
    else if (strcmp(argv[2], "2") == 0)
        led_num = 2;
    else if(strcmp(argv[2], "all") == 0)
        led_num = 3;



    //ioctl(fd, cmd, &led_num);
    ioctl(fd, cmd, led_num);
    
    
 
    return 0;
}



#else
int main(int argc, char *argv[])
{
    int fd = 0;
    unsigned int key_value[4]; 
    unsigned long cnt =0;
    
    fd = open("/dev/Button", O_RDWR);
    if(fd < 0)
        printf("can't open file \n");

        
    while(1)
    {
        read(fd, key_value, sizeof(key_value));

        if (!key_value[0] || !key_value[1] || !key_value[2] || !key_value[3])
        {
            printf("%04d key pressed: %d %d %d %d\n", cnt++, key_value[0], key_value[1], key_value[2], key_value[3]);
        }
    }

    return 0;
}

#endif
