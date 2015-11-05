#include <linux/init.h>
#include <linux/module.h>
#include <asm/hardware.h>
#include <asm/arch/gpio.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/device.h>


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




static struct class *first_driver_class;
static struct class_device *first_class_device;


static int first_driver_open(struct inode *inode, struct file *filp)
{
    printk("first_driver_open\n");
    return 0;
}


static ssize_t first_driver_write(struct file * file, const char * buf, size_t count, loff_t *off)
{
	ssize_t retval = count;

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


