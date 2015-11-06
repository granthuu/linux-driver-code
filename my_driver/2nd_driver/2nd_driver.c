#include <linux/init.h>
#include <linux/module.h>
#include <asm/hardware.h>
#include <asm/arch/gpio.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/device.h>
#include <linux/uaccess.h>


volatile unsigned long * gpfcon;
volatile unsigned long * gpgcon;

volatile unsigned long * gpfdat;
volatile unsigned long * gpgdat;

static struct class *second_driver_class;
static struct class_device * second_driver_class_device;


///////////////////////////////////////////key module.
/* 配置GPF0,2为输入引脚 */
/* 配置GPG3,11为输入引脚 */

#define CONFIG_KEY_GPIO()  do{  \ 
    *gpfcon &= ~((0x3<<(0*2)) | (0x3<<(2*2))); \                            	
    *gpgcon &= ~((0x3<<(3*2)) | (0x3<<(11*2))); \
}while(0)


////////////////led module

#define LED_CONFIG()    do{                                                             \
                            *gpfcon &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));	\ 
                            *gpfcon |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));   \
                        }while(0)

        
#define ALL_LED_ON()    (*gpfdat &= ~((1<<4) | (1<<5) | (1<<6)))
#define ALL_LED_OFF()   (*gpfdat |= ((1<<4) | (1<<5) | (1<<6)))

/* X = 0, 1, 2 */
#define LED_ON_NUM(X)     (*gpfdat &= ~(1<<(4+X)))
#define LED_OFF_NUM(X)    (*gpfdat |=  (1<<(4+X)))


static int second_driver_open(struct inode *inode, struct file *filp)
{
    printk("first_driver_open\n");

    // 1. initialize key GPIO.
    CONFIG_KEY_GPIO();
    LED_CONFIG();

    return 0;
}

static ssize_t second_driver_read( struct file *file, char *buf, size_t count, loff_t *ppos )
{
    unsigned int keyBuff[4];
	int regval;
    int len;
    
	if (count != sizeof(keyBuff))
		return -EINVAL;

	/* 读GPF0,2, 按键按下，则为0 */
	regval = *gpfdat;
	keyBuff[0] = (regval & (1<<0)) ? 1 : 0;
	keyBuff[1] = (regval & (1<<2)) ? 1 : 0;
	

	/* 读GPG3,11 */
	regval = *gpgdat;
	keyBuff[2] = (regval & (1<<3)) ? 1 : 0;
	keyBuff[3] = (regval & (1<<11)) ? 1 : 0; 
    

    len = copy_to_user(buf, keyBuff, sizeof(keyBuff));
    
    return sizeof(keyBuff);
}

static ssize_t second_driver_write(struct file * file, const char * buf, size_t count, loff_t *off)
{
	ssize_t retval = count;


	return retval;

}

#define LED_ON  1
#define LED_OFF 0
static int second_driver_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
//    unsigned int *ptr = (unsigned int *)arg;
//    unsigned int led_num = *ptr;

    unsigned int led_num = (unsigned int)arg;
    
    printk("arg: %d \n", led_num);
    switch(cmd)
    {
        case LED_ON: 
        
            if(led_num == 3)
                ALL_LED_ON();
            else
                LED_ON_NUM(led_num);
                
        break;

        case LED_OFF:
            if(led_num == 3)
                ALL_LED_OFF();
            else
                LED_OFF_NUM(led_num);
        break;

        default:
        break;
    }
    
    
    return 0;
}

static struct  file_operations second_driver_fops ={

    .owner = THIS_MODULE,
    .read  = second_driver_read,
    .write = second_driver_write,
    .open  = second_driver_open,
    .ioctl = second_driver_ioctl,

};


// 1. init

int major;

static int second_driver_init(void)
{
    major = register_chrdev(0, "second_driver", &second_driver_fops);

    second_driver_class = class_create(THIS_MODULE, "second_driver_class");
    second_driver_class_device = class_device_create(second_driver_class, NULL, MKDEV(major, 0), NULL, "Button");

    // key remap
	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;

	gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
	gpgdat = gpgcon + 1;


    printk("second_driver_init \n");

    return 0;
}


// 2. exit
static void second_driver_exit(void)
{
    unregister_chrdev(major, "second_driver");
    class_destroy(second_driver_class);
    class_device_del(second_driver_class_device);

    iounmap(gpfcon);
    iounmap(gpgcon);


    printk("second_driver_exit \n");
}


/* 3. GPL license */
module_init(second_driver_init);
module_exit(second_driver_exit);


MODULE_LICENSE("GPL");



