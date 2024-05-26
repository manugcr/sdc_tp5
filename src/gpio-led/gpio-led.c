#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
 
#define DEVICE_NAME "gpio-led"
#define GPIO_LED 17 // GPIO pin number for the LED
 
// Define GPIO base addresses
#define BCM2837_GPIO_ADDRESS 0x3F200000
#define BCM2711_GPIO_ADDRESS 0xfe200000
 
static int major_number;
static unsigned int *gpio_registers = NULL;
 
static void gpio_pin_on(unsigned int pin)
{
    unsigned int fsel_index = pin / 10;
    unsigned int fsel_bitpos = pin % 10;
    unsigned int* gpio_fsel = gpio_registers + fsel_index;
    unsigned int* gpio_on_register = (unsigned int*)((char*)gpio_registers + 0x1c);
 
    *gpio_fsel &= ~(7 << (fsel_bitpos * 3));
    *gpio_fsel |= (1 << (fsel_bitpos * 3));
    *gpio_on_register |= (1 << pin);
 
    printk(KERN_INFO "GPIO LED: Pin %d turned ON\n", pin);
}
 
static void gpio_pin_off(unsigned int pin)
{
    unsigned int *gpio_off_register = (unsigned int*)((char*)gpio_registers + 0x28);
    *gpio_off_register |= (1 << pin);
 
    printk(KERN_INFO "GPIO LED: Pin %d turned OFF\n", pin);
}
 
static int device_open(struct inode *inode, struct file *file) 
{
    printk(KERN_INFO "GPIO LED: Device opened\n");
    return 0;
}
 
static int device_release(struct inode *inode, struct file *file) 
{
    printk(KERN_INFO "GPIO LED: Device closed\n");
    return 0;
}
 
static ssize_t device_write(struct file *file, const char *buffer, size_t len, loff_t *offset) 
{
    int value;
 
    if (len > 0) {
        if (copy_from_user(&value, buffer, sizeof(int))) {
            printk(KERN_ALERT "GPIO LED: Failed to copy from user\n");
            return -EFAULT;
        }
 
        printk(KERN_INFO "GPIO LED: Writing value %d to GPIO %d\n", value, GPIO_LED);
 
        if (value) {
            gpio_pin_on(GPIO_LED);
        } else {
            gpio_pin_off(GPIO_LED);
        }
    }
    return len;
}
 
static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .write = device_write,
};
 
static int __init gpio_led_init(void) 
{
    printk(KERN_INFO "GPIO LED: Initializing\n");
 
    gpio_registers = (unsigned int*)ioremap(BCM2837_GPIO_ADDRESS, PAGE_SIZE);
    if (!gpio_registers) {
        printk(KERN_ALERT "GPIO LED: Failed to map GPIO memory\n");
        return -ENOMEM;
    }
 
    printk(KERN_INFO "GPIO LED: Successfully mapped GPIO memory\n");
 
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "GPIO LED: Failed to register a major number\n");
        iounmap(gpio_registers);
        return major_number;
    }
 
    printk(KERN_INFO "GPIO LED: Registered correctly with major number %d\n", major_number);
    return 0;
}
 
static void __exit gpio_led_exit(void) 
{
    gpio_pin_off(GPIO_LED);
    iounmap(gpio_registers);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "GPIO LED: Module unloaded\n");
}
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple GPIO LED driver for Raspberry Pi with direct register manipulation");
MODULE_VERSION("0.1");
 
module_init(gpio_led_init);
module_exit(gpio_led_exit);
