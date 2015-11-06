
/****************************************************************************************
   Copyright (c), 2009 ~ 2015 长沙智诚嵌入式技术有限公司
   文件名: ledtest.c
   作者: 曹国辉        
   版本: v1.0       
   日期: 2012-9-7

   说明: LED 驱动测试程序
   
   历史记录:
   
   智诚嵌入式培训 专注于大学生高薪就业培训
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


