set -e

arm-linux-gnueabihf-gcc -g -mcpu=arm7 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -o hello32.out hello.c --static
qemu-arm -L /usr/arm-linux-gnueabihf -g 1234 ./hello32.out


# arm-none-eabi-gcc -g -mcpu=cortex-m4 -MMD -MP -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -ffunction-sections -fdata-sections -fstack-usage -std=gnu11 -o hello32.elf hello32.c --specs=nosys.specs
# arm-none-eabi-gcc -g -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -std=gnu11 -o hello32.elf hello32.c --specs=nosys.specs