#!/bin/bash

MODULE_NAME="gpio-signal"
DEVICE_PATH="/dev/gpio-signal"
MAJOR_NUMBER=""

echo "  -> Unloading the module (if loaded)..."
sudo rmmod $MODULE_NAME 2>/dev/null

echo "  -> Removing the device node..."
sudo rm $DEVICE_PATH 2>/dev/null

echo "  -> Cleaning up build files..."
make clean

echo "Done."
