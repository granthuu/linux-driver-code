#include <linux/init.h>
#include <linux/module.h>
#include <asm/hardware.h>
#include <asm/arch/gpio.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/device.h>

#include <linux/uaccess.h>

//#include <linux/init.h>
//#include <linux/module.h>
//#include <linux/kernel.h>
//#include <linux/fs.h>
//#include <linux/delay.h>
//#include <asm/uaccess.h>
//#include <asm/irq.h>
//#include <asm/io.h>
//#include <asm/arch/regs-gpio.h>
//#include <asm/hardware.h>
//#include <linux/device.h>



static volatile unsigned long * gpfcon;
static volatile unsigned long * gpfdat;

static struct class *first_driver_class;
static struct class_device *first_class_device;


#define LED_CONFIG()    do{                                                             \
                            *gpfcon &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));	\ 
                            *gpfcon |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));   \
                        }while(0)

        
#define ALL_LED_ON()    (*gpfdat &= ~((1<<4) | (1<<5) | (1<<6)))
#define ALL_LED_OFF()   (*gpfdat |= ((1<<4) | (1<<5) | (1<<6)))

static int first_driver_open(struct inode *inode, struct file *filp)
{
    //printk("first_driver_open\n");
    
    	/* 配置GPF4,5,6为输出 */
//	*gpfcon &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));
//	*gpfcon |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));
	LED_CONFIG();
	
    return 0;
}


static ssize_t first_driver_write(struct file * file, const char * buf, size_t count, loff_t *off)
{
	ssize_t retval = count;
    int val = 0xff;
    unsigned long len = 0;
    
    len = copy_from_user(&val, buf, count);

    if(val == 1)
    {
        // turn on LEDs
        ALL_LED_ON();
    }
    else
    {
        // turn off LEDs
        ALL_LED_OFF();

    }

	return retval;
}


const struct file_operations first_driver_fops = {
	.owner       = THIS_MODULE,
	.write       = first_driver_write,
	.open        = first_driver_open,
};


/* 1. 入口函数 */
unsigned int major;


static int first_driver_init(void)
{
    // register char device.
	major = register_chrdev(0, "first_driver", &first_driver_fops);
	
	first_driver_class = class_create(THIS_MODULE, "first_driver");
	//class_device_create(first_driver_class, NULL, MKDEV(major, 0), first_class_device, "led");
	first_class_device = class_device_create(first_driver_class, NULL, MKDEV(major, 0), NULL, "led");

    // add LED gpio initialize, remap GPIO to kernel.
    // 16: indicate 16 bytes, remap 4 registers(32bits width).
	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;


    printk("first_driver_init\n");
    
	return 0;
}


/* 2. 出口函数*/
static void first_driver_exit(void)
{
	unregister_chrdev(major, "first_driver");
	class_device_unregister(first_class_device);
	class_destroy(first_driver_class);
    
    //class_device_destroy(struct class * cls, dev_t devt);

    printk("first_driver_exit\n");
}



/* 3. GPL license */
module_init(first_driver_init);
module_exit(first_driver_exit);


//MODULE_AUTHOR("Grant");
//MODULE_DESCRIPTION("My first driver.");
MODULE_LICENSE("GPL");


