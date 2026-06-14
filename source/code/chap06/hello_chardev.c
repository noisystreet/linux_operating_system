#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define DEVICE_NAME "hello_chardev"
#define BUF_SIZE 256

static char buffer[BUF_SIZE];
static int buf_len;
static dev_t dev_num;
static struct cdev char_cdev;
static struct class *char_class;

static ssize_t dev_read(struct file *f, char __user *user_buf,
                        size_t count, loff_t *offset)
{
    if (*offset >= buf_len)
        return 0;
    if (count > buf_len - *offset)
        count = buf_len - *offset;
    if (copy_to_user(user_buf, buffer + *offset, count))
        return -EFAULT;
    *offset += count;
    return count;
}

static ssize_t dev_write(struct file *f, const char __user *user_buf,
                         size_t count, loff_t *offset)
{
    if (count > BUF_SIZE - 1)
        count = BUF_SIZE - 1;
    if (copy_from_user(buffer, user_buf, count))
        return -EFAULT;
    buffer[count] = '\0';
    buf_len = count;
    *offset = count;
    return count;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .write = dev_write,
};

static int __init hello_init(void)
{
    int ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&char_cdev, &fops);
    ret = cdev_add(&char_cdev, dev_num, 1);
    if (ret < 0)
        goto err_cdev;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    char_class = class_create(THIS_MODULE, DEVICE_NAME);
#else
    char_class = class_create(DEVICE_NAME);
#endif
    if (IS_ERR(char_class)) {
        ret = PTR_ERR(char_class);
        goto err_class;
    }

    device_create(char_class, NULL, dev_num, NULL, DEVICE_NAME);
    pr_info("hello_chardev: loaded, major=%d\n", MAJOR(dev_num));
    return 0;

err_class:
    cdev_del(&char_cdev);
err_cdev:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

static void __exit hello_exit(void)
{
    device_destroy(char_class, dev_num);
    class_destroy(char_class);
    cdev_del(&char_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("hello_chardev: unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("os-tutorial");
MODULE_DESCRIPTION("Minimal char device driver");
