CROSS-COMPILER = arm-none-eabi-

all: blink.bin

blink.bin: blink.c startup.c vector_table.s
	$(CROSS-COMPILER)gcc -Wall -mcpu=cortex-m4 -mthumb -nostartfiles -T stm32f4.ld blink.c startup.c vector_table.s -o blink.elf
	$(CROSS-COMPILER)objcopy -O binary blink.elf blink.bin

flash:
	st-flash --reset write blink.bin 0x8000000

clean:
	rm -f *.o *.elf *.bin
