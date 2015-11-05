#include <linux/init.h>
#include <linux/module.h>
#include <asm/hardware.h>
#include <asm/arch/gpio.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/device.h>
#include <linux/uaccess.h>



static struct class *second_driver_class;
static struct class_device * second_driver_class_device;

static int second_driver_open(struct inode *inode, struct file *filp)
{
    printk("first_driver_open\n");
    

	
    return 0;
}

static ssize_t second_driver_read( struct file *file, char *buf, size_t count, loff_t *ppos )
{

    return count;
}

static ssize_t second_driver_write(struct file * file, const char * buf, size_t count, loff_t *off)
{
	ssize_t retval = count;


	return retval;

}


static struct  file_operations second_driver_fops ={

    .owner = THIS_MODULE,
    .read  = second_driver_read,
    .write = second_driver_write,
    .open  = second_driver_open,

};


// 1. init

int major;

static int second_driver_init(void)
{
    major = register_chrdev(0, "second_driver", &second_driver_fops);

    second_driver_class = class_create(THIS_MODULE, "second_driver_class");
    second_driver_class_device = class_device_create(second_driver_class, NULL, MKDEV(major, 0), NULL, "Button");

    printk("second_driver_init \n");

    return 0;
}


// 2. exit
static void second_driver_exit(void)
{
    unregister_chrdev(major, "second_driver");
    class_destroy(second_driver_class);
    class_device_del(second_driver_class_device);

    printk("second_driver_exit \n");
}


/* 3. GPL license */
module_init(second_driver_init);
module_exit(second_driver_exit);


MODULE_LICENSE("GPL");



