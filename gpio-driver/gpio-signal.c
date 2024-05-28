/**
 * gpio-signal.c - A simple GPIO INPUT driver for Raspberry Pi
 * 
 * This CDD raspberry pi driver allows to sense the state of two external signals 
 * with a period of one second connected to two GPIO pins. Then a user level application
 * can read ONE of this two signals and graph the result in time.
 * 
 * The driver must be able to tell the CDD which of the two signals to read.
 * 
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/moduleparam.h>

#define DEVICE_NAME "gpio-signal"
#define GPIO_INPUT1 22
#define GPIO_INPUT2 27
#define GPIO_OUTPUT 17

static int gpio_pin1 = GPIO_INPUT1;
static int gpio_pin2 = GPIO_INPUT2;
module_param(gpio_pin1, int, 0444);
MODULE_PARM_DESC(gpio_pin1, "GPIO input pin number 1 (default 22)");
module_param(gpio_pin2, int, 0444);
MODULE_PARM_DESC(gpio_pin2, "GPIO input pin number 2 (default 27)");

// Define GPIO base addresses
#define BCM2837_GPIO_ADDRESS 0x3F200000

// Function prototypes
static void gpio_pin_input(unsigned int pin);
static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *file, char *buffer, size_t len, loff_t *offset);
static ssize_t device_write(struct file *file, const char *buffer, size_t len, loff_t *offset);
static int __init gpio_signal_init(void);
static void __exit gpio_signal_exit(void);

// Global variables
static int major_number             = 0;
static unsigned int selected_pin    = 0;
static unsigned int *gpio_registers = NULL;

// File operations structure (needed for character device)
static struct file_operations fops = {
	.open       = device_open,
	.release    = device_release,
	.read       = device_read,
	.write      = device_write,
};

/**
 * @brief Device read function.
 * 
 * This function is called when the device file is read to get the value of the selected pin.
 * 
 * @param file Pointer to the file structure.
 * @param buffer Pointer to the buffer where the data will be written.
 * @param len Length of the buffer.
 * @param offset Pointer to the offset in the file.
 * @return Number of bytes read.
*/
static ssize_t device_read(struct file *file, char *buffer, size_t len, loff_t *offset)
{
    unsigned int gpio_value;
    char value_str[3];
    size_t value_str_len;

    gpio_value = (*(gpio_registers + 13) & (1 << selected_pin)) != 0;
    snprintf(value_str, sizeof(value_str), "%d\n", gpio_value);
    value_str_len = strlen(value_str);

    if (*offset >= value_str_len)
        return 0;

    if (len > value_str_len - *offset)
        len = value_str_len - *offset;

    if (copy_to_user(buffer, value_str + *offset, len))
        return -EFAULT;

    *offset += len;
    
    printk(KERN_INFO "GPIO SIGNAL: Value read %u.\n", gpio_value);
    
    return len;
}

/**
 * @brief Device write function.
 * 
 * This function is called when the device file is written to select the pin to read the signal
 * 
 * @param file Pointer to the file structure.
 * @param buffer Pointer to the buffer where the data is read.
 * @param len Length of the buffer.
 * @param offset Pointer to the offset in the file.
 * @return Number of bytes written.
*/
static ssize_t device_write(struct file *file, const char *buffer, size_t len, loff_t *offset)
{
    char kbuf[2];

    if (len > 1) len = 1;

    if (copy_from_user(kbuf, buffer, len))
        return -EFAULT;

    if (kbuf[0] == '1') {
        selected_pin = gpio_pin1;
    } else if (kbuf[0] == '2') {
        selected_pin = gpio_pin2;
    } else {
        return -EINVAL;
    }

    printk(KERN_INFO "GPIO SIGNAL: Selected pin %u.\n", selected_pin);

    return len;
}

/**
 * @brief Set the GPIO pin as input.
 * 
 * @param pin The GPIO pin number to set as input.
 * @return void
*/
static void gpio_pin_input(unsigned int pin)
{
    unsigned int fsel_index		= pin / 10;
    unsigned int fsel_bitpos		= pin % 10;
    unsigned int *gpio_fsel		= gpio_registers + fsel_index;

    *gpio_fsel	&=~	(7 << (fsel_bitpos * 3));	// Clear the bits for the pin
    *gpio_fsel	|=	(0 << (fsel_bitpos * 3));	// Set the pin as input
    
    printk(KERN_INFO "GPIO SIGNAL: Pin %d set up as input.\n", pin);
}

/**
 * @brief Device open function.
 *
 * This function is called when the device file is opened.
 *
 * @param inode Pointer to the inode structure.
 * @param file Pointer to the file structure.
 * @return 0 on success.
 */
static int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "GPIO SIGNAL: Device opened.\n");
	return 0;
}

/**
 * @brief Device release function.
 *
 * This function is called when the device file is closed.
 *
 * @param inode Pointer to the inode structure.
 * @param file Pointer to the file structure.
 * @return 0 on success.
 */
static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "GPIO SIGNAL: Device closed.\n");
	return 0;
}

/**
 * @brief Module initialization function.
 *
 * This function is called when the module is loaded. It initializes the
 * GPIO registers and registers the character device.
 *
 * @return 0 on success, or a negative error code on failure.
 */
static int __init gpio_signal_init(void)
{
	printk(KERN_INFO "GPIO SIGNAL: Initializing.\n");

	// Map GPIO memory
	gpio_registers = (unsigned int *)ioremap(BCM2837_GPIO_ADDRESS, PAGE_SIZE);
	if (!gpio_registers)
	{
		printk(KERN_ALERT "GPIO SIGNAL: Failed to map GPIO memory.\n");
		return -ENOMEM;
	}

	printk(KERN_INFO "GPIO SIGNAL: Successfully mapped GPIO memory.\n");

	gpio_pin_input(gpio_pin1);
	gpio_pin_input(gpio_pin2);
	selected_pin = gpio_pin1;	// Default selected pin

	// Register character device
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0)
	{
		printk(KERN_ALERT "GPIO SIGNAL: Failed to register a major number.\n");
		iounmap(gpio_registers);
		return major_number;
	}

	printk(KERN_INFO "GPIO SIGNAL: Registered correctly with major number %d.\n", major_number);
	return 0;
}

/**
 * @brief Module exit function.
 *
 * This function is called when the module is unloaded. It unregisters the
 * character device and unmaps the GPIO memory.
 */
static void __exit gpio_signal_exit(void)
{
	iounmap(gpio_registers);
	unregister_chrdev(major_number, DEVICE_NAME);
	printk(KERN_INFO "GPIO SIGNAL: Module unloaded.\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manu, Palla, Sapo");
MODULE_DESCRIPTION("A simple GPIO INPUT driver for Raspberry Pi");
MODULE_VERSION("1.0");

module_init(gpio_signal_init);
module_exit(gpio_signal_exit);
