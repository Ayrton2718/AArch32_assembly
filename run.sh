#!/bin/bash

set -e

arm-linux-gnueabihf-gcc -g -mfpu=fpv4-sp-d16 -mfloat-abi=hard -o main.out main.c tiny_mat.c --static

if [ $# -eq 0 ]; then
    qemu-arm -L /usr/arm-linux-gnueabihf ./main.out
else
    if [ "$1" = "-g" ]; then
        qemu-arm -L /usr/arm-linux-gnueabihf -g 1234 ./main.out
    fi
fi


# arm-none-eabi-gcc -g -mcpu=cortex-m4 -MMD -MP -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -ffunction-sections -fdata-sections -fstack-usage -std=gnu11 -o hello32.elf hello32.c --specs=nosys.specs
# arm-none-eabi-gcc -g -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -std=gnu11 -o hello32.elf hello32.c --specs=nosys.specs