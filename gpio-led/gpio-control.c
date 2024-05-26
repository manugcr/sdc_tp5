#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
 
#define DEVICE_PATH "/dev/gpio-led"
 
void print_usage(const char *prog_name) 
{
    printf("Usage: %s <on|off>\n", prog_name);
}
 
int main(int argc, char *argv[]) 
{
    int fd;
    int value;
 
    if (argc != 2) 
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
 
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
 
    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) 
    {
        perror("Failed to open device file");
        return EXIT_FAILURE;
    }
 
    if (write(fd, &value, sizeof(value)) < 0) 
    {
        perror("Failed to write to device file");
        close(fd);
        return EXIT_FAILURE;
    }
 
    printf("LED turned %s successfully\n", argv[1]);
 
    close(fd);
    return EXIT_SUCCESS;
}
