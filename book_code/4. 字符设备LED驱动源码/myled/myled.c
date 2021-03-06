

/****************************************************************************************
   Copyright (c), 2009 ~ 2015 长沙智诚嵌入式技术有限公司
   文件名: myled.c
   作者: 曹国辉        
   版本: v1.0       
   日期: 2012-9-7

   说明: LED Linux驱动程序
   
   历史记录:
   
   智诚嵌入式培训 专注于大学生高薪就业培训
   http://www.jslinux.com 
   
******************************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <mach/regs-gpio.h>
#include <asm/io.h>


/*define myled main and minor device number*/
#define MAINLEDNO      108
#define MINORLEDNO    0


#define LED1_ON  (0<<5)
#define LED2_ON  (0<<6)
#define LED3_ON  (0<<7)
#define LED4_ON  (0<<8)

#define LED1_OFF  (1<<5)
#define LED2_OFF  (1<<6)
#define LED3_OFF  (1<<7)
#define LED4_OFF  (1<<8)


#define LED1_BIT   (1<<5)
#define LED2_BIT   (1<<6)
#define LED3_BIT   (1<<7)
#define LED4_BIT   (1<<8)


static unsigned int MYLED_BIT_ARRY[4] = {0x20,0x40,0x80,0x100};

/*define  myled device struct, it include cdev struct, present myled device*/

struct myled_dev {
   char name[20];
   struct cdev cdev;
} ;



struct  myled_dev   myled_dev0; /*define myled1 as myled device*/
int myled_no  = 0;   /*define myled device number*/

int myled_open(struct inode *inode, struct file *file)
{
     unsigned int value1 = 0;

    value1 = (1<<10)+(1<<12)+(1<<14)+(1<<16); /*config GPB5,6,7,8 as output */
    writel( value1, S3C2410_GPBCON);

     value1 = 0x61f;  /*enable  GPB5,6,7,8 up*/
     writel( value1, S3C2410_GPBUP);

    value1 = (LED1_OFF + LED2_OFF + LED3_OFF + LED4_OFF); /*turn all led off*/
    writel( value1, S3C2410_GPBDAT);

     printk("open myled ok\n");
    return 0;

}


#define CMD_TURN_ON_LED       0x01
#define CMD_TURN_OFF_LED      0x02

/*arg , is unsigned int *         
low 4 bit is significiant, 

bit 0 --- LED0
bit 1 --- LED1
bit 2 --- LED2
bit 3 --- LED3


*/

int myled_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
   
    unsigned int *pled_num = NULL;
    unsigned int   led_num = 0;
    unsigned int    itmp = 1;
    int i = 0;

     unsigned int value = readl(S3C2410_GPBDAT);
	
     pled_num = (unsigned int *)arg;  /*arg present  address*/
     led_num =  *pled_num;

     printk("k led num: %x", led_num);

     led_num &= 0x0f; 
	 
	
     switch(cmd)
     {
          case CMD_TURN_ON_LED:
                 printk("cmd led on value: %x\n",value );

		   for (i = 0; i < 4; i++)
		   {
		   	 if(led_num&itmp)
		   	 {
                              printk("on enter value : %x, arry: %x i:%d \n", value, MYLED_BIT_ARRY[i],i);
		   	     value &= (~MYLED_BIT_ARRY[i]);
		   	 }
                        printk("value : %x ARRY:%x \n", value, MYLED_BIT_ARRY[i]);

		        itmp = itmp << 1; 
		   }
			
	   break;

	   case CMD_TURN_OFF_LED:

		  for (i = 0; i < 4; i++)
		   {
		   	 if(led_num&itmp)
		   	 {
                              printk(" off enter value : %x, arry %x \n", value, MYLED_BIT_ARRY[i]); 
		   	     value |= MYLED_BIT_ARRY[i];
		   	 }

			 itmp = itmp << 1;
                       printk("value : %x  ARRY: %x \n", value, MYLED_BIT_ARRY[i]);
 
		   }
		   
	          break;

	   default: 	
	         break;
	  	  	
     }

  printk ("valute to register : %x\n", value);
  writel(value, S3C2410_GPBDAT);

   return 0;
    
}

int myled_close(struct inode *inode, struct file *file)
{
     printk("close myled ok\n");
     return 0;
}

struct file_operations  led_ops = {
 
      .open=myled_open,
      .ioctl =myled_ioctl, 
      .release = myled_close,
	
};


static int __init myled_init(void)
{
     /*register myled device */

     int iret = 0;

     myled_no = MKDEV(MAINLEDNO, MINORLEDNO);
	 
   /*get device number,check device number is ok */
     iret = register_chrdev_region(myled_no, 1, "myled");
    if(iret < 0)
    {
        printk("register error, num: %d have been used!\n", myled_no);
	return iret;
    }

    cdev_init(&myled_dev0.cdev , &led_ops); /*init myled_dev0 device*/
    myled_dev0.cdev.owner = THIS_MODULE;
    
    iret = cdev_add(&myled_dev0.cdev, myled_no, 1);
    if(iret < 0)
    {
           printk("add myled_dev0 error!\n");   
	    return iret;
    }
	
    printk("myled init ok\n");    
    return 0; 
	
}

static void __exit myled_exit(void)
{
    cdev_del(&myled_dev0.cdev);
    unregister_chrdev_region(myled_no, 1);
    printk("myled exit ok\n");   
}


module_init(myled_init);
module_exit(myled_exit);

MODULE_LICENSE("GPL");



