
/****************************************************************************************
   Copyright (c), 2009 ~ 2015 ��ɳ�ǳ�Ƕ��ʽ�������޹�˾
   �ļ���: ledtest.c
   ����: �ܹ���        
   �汾: v1.0       
   ����: 2012-9-7

   ˵��: LED �������Գ���
   
   ��ʷ��¼:
   
   �ǳ�Ƕ��ʽ��ѵ רע�ڴ�ѧ����н��ҵ��ѵ
   http://www.jslinux.com 
   
******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>


#define CMD_TURN_ON_LED      0x01
#define CMD_TURN_OFF_LED     0x02

int main(int argc, char **argv)
{
   int fd;
   unsigned int cmd = 0;
   unsigned int	led_num = 0;

   printf("start test led\n");
   if(argc != 3)
   {
      printf("command parameter error \n");
      return 0;  
   }

  if (strcmp(argv[1], "on") == 0)
  {
       printf("led on\n");
       cmd = CMD_TURN_ON_LED;
  }
  else if(strcmp(argv[1], "off") == 0)
  {
        printf("led off\n");
     cmd = CMD_TURN_OFF_LED ;
  }
  else 
  {
      printf("command parameter error \n");
	  return 0 ;
  }



   led_num = (unsigned int) atoi(argv[2]);
   printf("lednum: %x", led_num);

   if(led_num > 0xf)
   {
     printf("command parameter error \n");
	return 0;
   }


   fd = open("/dev/ledS0", 0);

   ioctl(fd,  cmd, &led_num);
   close(fd);
 
   return 0;

}


