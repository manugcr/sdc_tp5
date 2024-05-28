#!/bin/bash
 
MODULE_NAME="gpio-signal"
DEVICE_PATH="/dev/gpio-signal"
MAJOR_NUMBER=""
 
echo "  -> Cleaning previous builds..."
make clean
 
echo "  -> Building the module..."
make
 
if [ $? -ne 0 ]; then
    echo "  -> Build failed. Exiting."
    exit 1
fi
 
echo "  -> Unloading the module (if loaded)..."
sudo rmmod $MODULE_NAME 2>/dev/null
 
echo "  -> Loading the new module..."
sudo insmod ./${MODULE_NAME}.ko gpio_pin1=22 gpio_pin2=27
 
if [ $? -ne 0 ]; then
    echo "  -> Failed to load the module. Exiting."
    exit 1
fi
 
MAJOR_NUMBER=$(awk "\$2==\"$MODULE_NAME\" {print \$1}" /proc/devices)
 
if [ -z "$MAJOR_NUMBER" ]; then
    echo "  -> Could not get major number. Exiting."
    exit 1
fi
 
echo "  -> Creating device node..."
sudo mknod $DEVICE_PATH c $MAJOR_NUMBER 0
 
echo "  -> Setting permissions on device node..."
sudo chmod 666 $DEVICE_PATH
 
echo "Done."
