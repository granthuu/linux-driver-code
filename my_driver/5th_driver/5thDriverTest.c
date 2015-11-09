#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>


/* fifthdrvtest
  */
int fd;

void my_signal_fun(int signum)
{
    unsigned char key_val;
    read(fd, &key_val, 1);
    printf("key_val: 0x%x\n", key_val);
}

int main(int argc, char **argv)
{
    unsigned char key_val;
    int ret;
    int Oflags;

    
    fd = open("/dev/key", O_RDWR);
    if (fd < 0)
    {
        printf("can't open!\n");
    }


    /**  
       void (*signal(int signum,void(* handler)(int)))(int);
    *  第一个参数signum指明了所要处理的信号类型
    *  第二个参数handler描述了与信号关联的动作
    **/
    signal(SIGIO, my_signal_fun);    // 驱动程序里面，当有按键按下时，发送一个SIGIO信号，应用程序调用回调函数my_signal_fun
    
    fcntl(fd, F_SETOWN, getpid());        // 指定当前进程作为文件fd（设备文件）的owner，目的是让内核（内核操作设备文件）知道应该通知那个进程
    Oflags = fcntl(fd, F_GETFL);          // 获取标志位
    fcntl(fd, F_SETFL, Oflags | FASYNC);  // 启用异步通知机制


    while (1)
    {
        sleep(1000);
    }
    
    return 0;
} 

