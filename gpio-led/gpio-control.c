#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define DEVICE_PATH "/dev/gpio-led"

/**
 * @brief Print the usage message.
 *
 * This function prints a usage message to the standard output.
 *
 * @param prog_name The name of the program.
 */
void print_usage(const char *prog_name)
{
    printf("Usage: %s <on|off>\n", prog_name);
}

/**
 * @brief Main function.
 *
 * This function is the entry point of the program. It takes a command-line argument to turn the LED on or off,
 * opens the device file, writes the appropriate value to it, and then closes the file.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return EXIT_SUCCESS on success, or EXIT_FAILURE on failure.
 */
int main(int argc, char *argv[])
{
    int fd;
    int value;

    // Check if the correct number of arguments is provided
    if (argc != 2)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Determine the value to write based on the argument
    if (strcmp(argv[1], "on") == 0)
    {
        value = 1;
    }
    else if (strcmp(argv[1], "off") == 0)
    {
        value = 0;
    }
    else
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Open the device file for writing
    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open device file.");
        return EXIT_FAILURE;
    }

    // Write the value to the device file
    if (write(fd, &value, sizeof(value)) < 0)
    {
        perror("Failed to write to device file.");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("LED turned %s successfully.\n", argv[1]);

    close(fd);
    return EXIT_SUCCESS;
}
