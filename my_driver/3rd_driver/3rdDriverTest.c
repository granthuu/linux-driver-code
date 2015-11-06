#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


int main(int argc, char **argv)
{
    int fd = 0;
    unsigned long cnt = 0;
    unsigned char key_value =0;

    
    fd = open("/dev/key", O_RDWR);
    

    while(1)
    {
        read(fd, &key_value, 1);   
        if(key_value)  
        {
            printf("cnt: %d, key: 0x%x \n", cnt++, key_value);
            //key_value = 0;
        }
        printf("hello world\n");
    }

    return 0;
}


