CROSS-COMPILER = arm-none-eabi-

all: asm01.elf

asm01.elf: asm01.s
	$(CROSS-COMPILER)gcc -mcpu=cortex-m4 -nostartfiles -Ttext=0x0 asm01.s -o asm01.elf
	$(CROSS-COMPILER)objdump -D asm01.elf

clean:
	rm -rf *.o *.elf
