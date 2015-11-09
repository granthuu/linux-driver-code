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
    *  ��һ������signumָ������Ҫ������ź�����
    *  �ڶ�������handler���������źŹ����Ķ���
    **/
    signal(SIGIO, my_signal_fun);    // �����������棬���а�������ʱ������һ��SIGIO�źţ�Ӧ�ó�����ûص�����my_signal_fun
    
    fcntl(fd, F_SETOWN, getpid());        // ָ����ǰ������Ϊ�ļ�fd���豸�ļ�����owner��Ŀ�������ںˣ��ں˲����豸�ļ���֪��Ӧ��֪ͨ�Ǹ�����
    Oflags = fcntl(fd, F_GETFL);          // ��ȡ��־λ
    fcntl(fd, F_SETFL, Oflags | FASYNC);  // �����첽֪ͨ����


    while (1)
    {
        sleep(1000);
    }
    
    return 0;
} 

