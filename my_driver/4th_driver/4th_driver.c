#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>

// grant add.
#include <linux/interrupt.h> 
#include <linux/poll.h>


static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

/* �ж��¼���־, �жϷ����������1��third_drv_read������0 */
static volatile int ev_press = 0;


static int major;
static struct class * third_driver_class;
static struct class_device * third_driver_class_device;

volatile unsigned long *gpfcon;
volatile unsigned long *gpfdat;

volatile unsigned long *gpgcon;
volatile unsigned long *gpgdat;



typedef struct pin_desc{
    unsigned int pin;
    unsigned int key_val;

}pin_desc;


pin_desc pins_desc[4] = {

    {S3C2410_GPF0,  0x01},
    {S3C2410_GPF2,  0x02},
    {S3C2410_GPG3,  0x03},
    {S3C2410_GPG11, 0x04},
};


unsigned char key_value = 0;


static irqreturn_t buttons_irq(int irq, void *dev_id)
{
    unsigned int value = 0;
    pin_desc *pin_desc_temp = (pin_desc *)dev_id;

    value = s3c2410_gpio_getpin(pin_desc_temp->pin);

    if(value)
    {
        // release key
        key_value = 0x80 | pin_desc_temp->key_val;
        
    }
    else
    {
        // press key
        key_value = pin_desc_temp->key_val;
        
    }

    ev_press = 10;                           /* ��ʾ�жϷ����� */
    wake_up_interruptible(&button_waitq);   /* �������ߵĽ��� */

    //printk("buttons_irq \n");
    
    return IRQ_HANDLED;
}


static int forth_driver_open(struct inode *inode, struct file *filp)
{
    int ignoreWarning_parameter = 0;
    
    printk("first_driver_open\n");

    // register key interrupt handler.
    ignoreWarning_parameter = request_irq(IRQ_EINT0,  buttons_irq, IRQT_BOTHEDGE, "S2", &pins_desc[0]);
    ignoreWarning_parameter = request_irq(IRQ_EINT2,  buttons_irq, IRQT_BOTHEDGE, "S3", &pins_desc[1]);
    ignoreWarning_parameter = request_irq(IRQ_EINT11, buttons_irq, IRQT_BOTHEDGE, "S4", &pins_desc[2]);
    ignoreWarning_parameter = request_irq(IRQ_EINT19, buttons_irq, IRQT_BOTHEDGE, "S5", &pins_desc[3]);
    
    return 0;
}

static int forth_driver_close(struct inode *inode, struct file *file)
{
    free_irq(IRQ_EINT0,  &pins_desc[0]);
    free_irq(IRQ_EINT2,  &pins_desc[1]);
    free_irq(IRQ_EINT11, &pins_desc[2]);
    free_irq(IRQ_EINT19, &pins_desc[3]);
    
	return 0;
}


static ssize_t forth_driver_read( struct file *file, char *buf, size_t count, loff_t *ppos )
{
    unsigned long temp = 0;
    
	if (count < 1)
		return -EINVAL;

	/* ���û�а�������, ���� */
	wait_event_interruptible(button_waitq, ev_press);
    //printk("third_driver_read \n");

	/* ����а�������, ���ؼ�ֵ */
	temp = copy_to_user(buf, &key_value, 1);
	ev_press = 0;

//    temp = copy_to_user(buf, &key_value, sizeof(key_value));
//    key_value = 0x00;
    

    return count;
		
}


static unsigned int forth_drv_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	poll_wait(file, &button_waitq, wait); // ������������

	if (ev_press)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}




static struct file_operations third_fops = {
    .owner   = THIS_MODULE,
    .open    = forth_driver_open,
    .release = forth_driver_close,
    .read    = forth_driver_read,
    //.write = xxx.write,
    .poll    = forth_drv_poll,
};





static int forth_driver_init(void)
{
    // 1. register char device
    major = register_chrdev(0, "third driver", &third_fops);

    // 2. register class, and class device
    third_driver_class = class_create(THIS_MODULE, "third driver class");
    third_driver_class_device = class_device_create(third_driver_class, NULL, MKDEV(major, 0), NULL, "key");

    // 3. remap GPIO to virtual address.
    gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
    gpfdat = gpfcon + 1;
    
    gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
    gpgdat = gpgcon + 1;  
    
    return 0;
}


static void forth_driver_exit(void)
{
    unregister_chrdev(major, "third driver");
    class_destroy(third_driver_class);
    class_device_unregister(third_driver_class_device);


    // unmap GPIO
    iounmap(gpfcon);
    iounmap(gpgcon);
}



/* 3. GPL license */
module_init(forth_driver_init);
module_exit(forth_driver_exit);
MODULE_LICENSE("GPL");


