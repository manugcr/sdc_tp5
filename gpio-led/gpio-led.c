#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define DEVICE_NAME "gpio-led"
#define GPIO_LED 17 // GPIO pin number for the LED

// Define GPIO base addresses
#define BCM2837_GPIO_ADDRESS 0x3F200000

static int major_number;
static unsigned int *gpio_registers = NULL;


/**
 * @brief Turn on the GPIO pin.
 *
 * This function configures the specified GPIO pin as an output and then
 * sets it to a high state, effectively turning on the LED connected to it.
 *
 * @param pin The GPIO pin number to turn on.
 */
static void gpio_pin_on(unsigned int pin)
{
    unsigned int fsel_index = pin / 10;                         // Calculate the FSEL register index 
    unsigned int fsel_bitpos = pin % 10;                        // Calculate the bit position in the FSEL register
    unsigned int *gpio_fsel = gpio_registers + fsel_index;      // Pointer to the FSEL register
    // Pointer to the GPIO output set register
    unsigned int *gpio_on_register = (unsigned int *)((char *)gpio_registers + 0x1c);

    *gpio_fsel &= ~(7 << (fsel_bitpos * 3));                    // Clear the bits for the pin
    *gpio_fsel |= (1 << (fsel_bitpos * 3));                     // Set the pin as output
    *gpio_on_register |= (1 << pin);                            // Set the pin high

    printk(KERN_INFO "GPIO LED: Pin %d turned ON.\n", pin);
}

/**
 * @brief Turn off the GPIO pin.
 *
 * This function sets the specified GPIO pin to a low state, effectively
 * turning off the LED connected to it.
 *
 * @param pin The GPIO pin number to turn off.
 */
static void gpio_pin_off(unsigned int pin)
{
    // Pointer to the GPIO output clear register
    unsigned int *gpio_off_register = (unsigned int *)((char *)gpio_registers + 0x28);
    *gpio_off_register |= (1 << pin);       // Set the pin low

    printk(KERN_INFO "GPIO LED: Pin %d turned OFF.\n", pin);
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
    printk(KERN_INFO "GPIO LED: Device opened.\n");
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
    printk(KERN_INFO "GPIO LED: Device closed.\n");
    return 0;
}

/**
 * @brief Device write function.
 *
 * This function is called when data is written to the device file. It turns
 * the LED on or off based on the written value.
 * The value received from the user is a string.
 *
 * @param file Pointer to the file structure.
 * @param buffer Pointer to the user buffer containing the data to write.
 * @param len Number of bytes to write.
 * @param offset Pointer to the file offset.
 * @return The number of bytes written on success, or a negative error code on failure.
 */
static ssize_t device_write(struct file *file, const char *buffer, size_t len, loff_t *offset)
{
    char value_str[2];
    int value;

    if (len > 0)
    {
        if (copy_from_user(value_str, buffer, sizeof(value_str) - 1))
        {
            printk(KERN_ALERT "GPIO LED: Failed to copy from user.\n");
            return -EFAULT;
        }

        value_str[sizeof(value_str) - 1] = '\0';
        value = simple_strtol(value_str, NULL, 10);

        printk(KERN_INFO "GPIO LED: Writing value %d to GPIO %d.\n", value, GPIO_LED);

        if (value)
        {
            gpio_pin_on(GPIO_LED);
        }
        else if (value == 0)
        {
            gpio_pin_off(GPIO_LED);
        }
        else
        {
           printk(KERN_ALERT "GPIO LED: Invalid value %d.\n", value);
           return -EINVAL;
        }
    }
    return len;
}

/**
 * @brief File operations structure.
 *
 * This structure defines the file operations for the device.
 */
static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .write = device_write,
};

/**
 * @brief Module initialization function.
 *
 * This function is called when the module is loaded. It initializes the
 * GPIO registers and registers the character device.
 *
 * @return 0 on success, or a negative error code on failure.
 */
static int __init gpio_led_init(void)
{
    printk(KERN_INFO "GPIO LED: Initializing.\n");

    // Map GPIO memory
    gpio_registers = (unsigned int *)ioremap(BCM2837_GPIO_ADDRESS, PAGE_SIZE);
    if (!gpio_registers)
    {
        printk(KERN_ALERT "GPIO LED: Failed to map GPIO memory.\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "GPIO LED: Successfully mapped GPIO memory.\n");

    // Register character device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
    {
        printk(KERN_ALERT "GPIO LED: Failed to register a major number.\n");
        iounmap(gpio_registers);
        return major_number;
    }

    printk(KERN_INFO "GPIO LED: Registered correctly with major number %d.\n", major_number);
    return 0;
}

/**
 * @brief Module exit function.
 *
 * This function is called when the module is unloaded. It unregisters the
 * character device and unmaps the GPIO memory.
 */
static void __exit gpio_led_exit(void)
{
    gpio_pin_off(GPIO_LED);                             // Turn off the LED
    iounmap(gpio_registers);                            // Unmap GPIO memory
    unregister_chrdev(major_number, DEVICE_NAME);       // Unregister device
    printk(KERN_INFO "GPIO LED: Module unloaded.\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manu, Palla, Sapo");
MODULE_DESCRIPTION("A simple GPIO LED driver for Raspberry Pi with direct register manipulation");
MODULE_VERSION("1.0");

module_init(gpio_led_init);
module_exit(gpio_led_exit);
