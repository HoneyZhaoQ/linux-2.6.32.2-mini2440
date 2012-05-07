
/*
    this module is used to reset sufa over mini2440
    reset gpio pin is GPG0 pin

    when init register to device "/dev/sreset"

    to reset sufa use echo on linux user space:
        "echo -n 1 > /dev/sreset"
        "echo -n 0 > /dev/sreset"
*/


#include <linux/miscdevice.h>
#include <mach/regs-gpio.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "sreset"


/* no need to do anything */
int sufa1_reset_open(struct inode *inode, struct file *filp) 
{
  return 0;
}

/* no need to do anything */
int sufa1_reset_release(struct inode *inode, struct file *filp) 
{
  return 0;
}

ssize_t sufa1_reset_write( struct file *filp, char *buf, 
  size_t count, loff_t *f_pos)
{
    char pcVal[2] = {'\0'};
    int iCmd = 0;

    copy_from_user(pcVal, buf, 1);

    iCmd = (unsigned int)simple_strtol(pcVal, NULL, 10);
    if (iCmd != 0 && iCmd != 1)
        return -EINVAL;

    printk(DEVICE_NAME "\twrite value: %d\n", iCmd);
    s3c2410_gpio_setpin(S3C2410_GPG(0), !iCmd);

    return 1;
}

static struct file_operations dev_fops = {
    .owner  = THIS_MODULE,
    .open   = sufa1_reset_open,
    .release= sufa1_reset_release,
    .write  = sufa1_reset_write,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = DEVICE_NAME,
    .fops  = &dev_fops,
};

static int __init dev_init(void)
{
    int ret;

    s3c2410_gpio_cfgpin(S3C2410_GPG(0), S3C2410_GPIO_OUTPUT);
    s3c2410_gpio_setpin(S3C2410_GPG(0), 0);

    ret = misc_register(&misc);

    printk (DEVICE_NAME "\tinitialized\n");

    return ret;
}

static void __exit dev_exit(void)
{
    printk (DEVICE_NAME "\tmodule exit\n");
    misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tamara Elektronik Ltd.Sti.");
