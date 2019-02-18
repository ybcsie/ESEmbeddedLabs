#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include "reg.h"
#include "blink.h"
#include "asm_func.h"

#define HEAP_MAX (64 * 1024) //64 KB

void user_task(void);

void init_usart1(void);
void usart1_send_char(const char ch);

int main(void)
{
	extern uint32_t _msp_init;

	uint32_t *msp_init = &_msp_init;
	uint32_t *psp_init = msp_init - 8 * 1024;

	init_usart1();

	printf("[Kernel] Start in privileged thread mode (msp_init = 0x%x).\r\n\n", (unsigned int)msp_init);

	printf("[Kernel] Control: 0x%x \r\n", (unsigned int)read_ctrl());
	printf("[Kernel] SP: 0x%x \r\n", (unsigned int)read_sp());
	printf("[Kernel] MSP: 0x%x \r\n", (unsigned int)read_msp());
	printf("[Kernel] PSP: 0x%x \r\n\n", (unsigned int)read_psp());

	printf("[Kernel] Switch to unprivileged thread mode & start user task (psp_init = 0x%x).\r\n\n", (unsigned int)psp_init);

	//start user task
	??????

	while (1) //should not go here
		;
}

void user_task(void)
{
	printf("[User] Start in unprivileged thread mode.\r\n\n");

	printf("[User] Control: 0x%x \r\n", (unsigned int)read_ctrl());
	printf("[User] SP: 0x%x \r\n", (unsigned int)read_sp());
	printf("[User] MSP: 0x%x \r\n", (unsigned int)read_msp());
	printf("[User] PSP: 0x%x \r\n\n", (unsigned int)read_psp());

	printf("[User] Switch to privileged thread mode by writing the control register directly. (forbidden)\r\n\n");

	sw_priv();

	printf("[User] Control: 0x%x \r\n", (unsigned int)read_ctrl());
	printf("[User] SP: 0x%x \r\n", (unsigned int)read_sp());
	printf("[User] MSP: 0x%x \r\n", (unsigned int)read_msp());
	printf("[User] PSP: 0x%x \r\n\n", (unsigned int)read_psp());

	blink(LED_BLUE); //should not return
}

void init_usart1(void)
{
	//RCC EN GPIO
	SET_BIT(RCC_BASE + RCC_AHB1ENR_OFFSET, GPIO_EN_BIT(GPIO_PORTB));

	//MODER => 10
	SET_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_MODER_OFFSET, MODERy_1_BIT(6));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_MODER_OFFSET, MODERy_0_BIT(6));

	SET_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_MODER_OFFSET, MODERy_1_BIT(7));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_MODER_OFFSET, MODERy_0_BIT(7));

	//OT => 0
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OTYPER_OFFSET, OTy_BIT(6));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OTYPER_OFFSET, OTy_BIT(7));

	//OSPEEDR => 10
	SET_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OSPEEDR_OFFSET, OSPEEDRy_1_BIT(6));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OSPEEDR_OFFSET, OSPEEDRy_0_BIT(6));

	SET_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OSPEEDR_OFFSET, OSPEEDRy_1_BIT(7));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_OSPEEDR_OFFSET, OSPEEDRy_0_BIT(7));

	//PUPDR = 00 => No pull-up, pull-down
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_PUPDR_OFFSET, PUPDRy_1_BIT(6));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_PUPDR_OFFSET, PUPDRy_0_BIT(6));

	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_PUPDR_OFFSET, PUPDRy_1_BIT(7));
	CLEAR_BIT(GPIO_BASE(GPIO_PORTB) + GPIOx_PUPDR_OFFSET, PUPDRy_0_BIT(7));

	//AF sel
	WRITE_BITS(GPIO_BASE(GPIO_PORTB) + GPIOx_AFRL_OFFSET, AFRLy_3_BIT(6), AFRLy_0_BIT(6), 7);
	WRITE_BITS(GPIO_BASE(GPIO_PORTB) + GPIOx_AFRL_OFFSET, AFRLy_3_BIT(7), AFRLy_0_BIT(7), 7);

	//RCC EN USART1
	SET_BIT(RCC_BASE + RCC_APB2ENR_OFFSET, USART1EN);

	//baud rate
	const unsigned int BAUDRATE = 115200;
	const unsigned int SYSCLK_MHZ = 168;
	const double USARTDIV = SYSCLK_MHZ * 1.0e6 / 8 / 2 / BAUDRATE;

	const uint32_t DIV_MANTISSA = (uint32_t)USARTDIV;
	const uint32_t DIV_FRACTION = (uint32_t)((USARTDIV - DIV_MANTISSA) * 16);

	WRITE_BITS(USART1_BASE + USART_BRR_OFFSET, DIV_MANTISSA_11_BIT, DIV_MANTISSA_0_BIT, DIV_MANTISSA);
	WRITE_BITS(USART1_BASE + USART_BRR_OFFSET, DIV_FRACTION_3_BIT, DIV_FRACTION_0_BIT, DIV_FRACTION);

	//usart1 enable
	SET_BIT(USART1_BASE + USART_CR1_OFFSET, UE_BIT);

	//set TE
	SET_BIT(USART1_BASE + USART_CR1_OFFSET, TE_BIT);

	//set RE
	SET_BIT(USART1_BASE + USART_CR1_OFFSET, RE_BIT);

	//set RXNEIE
	SET_BIT(USART1_BASE + USART_CR1_OFFSET, RXNEIE_BIT);

	//set NVIC
	SET_BIT(NVIC_ISER_BASE + NVIC_ISERn_OFFSET(1), 5); //IRQ37 => (m+(32*n)) | m=5, n=1
}

void usart1_send_char(const char ch)
{
	//wait util TXE == 1
	while (!READ_BIT(USART1_BASE + USART_SR_OFFSET, TXE_BIT))
		;

	REG(USART1_BASE + USART_DR_OFFSET) = (uint8_t)ch;
}

void usart1_handler(void)
{
	if (READ_BIT(USART1_BASE + USART_SR_OFFSET, ORE_BIT))
	{
		for (unsigned int i = 0; i < 5000000; i++)
			;
		char ch = (char)REG(USART1_BASE + USART_DR_OFFSET);

		if (ch == '\r')
			usart1_send_char('\n');

		usart1_send_char(ch);
		usart1_send_char('~');
	}

	else if (READ_BIT(USART1_BASE + USART_SR_OFFSET, RXNE_BIT))
	{
		char ch = (char)REG(USART1_BASE + USART_DR_OFFSET);

		if (ch == '\r')
			usart1_send_char('\n');

		usart1_send_char(ch);
	}
}

void *_sbrk(int incr)
{
	extern uint8_t _mybss_vma_end; //Defined by the linker script
	static uint8_t *heap_end = NULL;
	uint8_t *prev_heap_end;

	if (heap_end == NULL)
		heap_end = &_mybss_vma_end;

	prev_heap_end = heap_end;
	if (heap_end + incr > &_mybss_vma_end + HEAP_MAX)
		return (void *)-1;

	heap_end += incr;
	return (void *)prev_heap_end;
}

int _write(int file, char *ptr, int len)
{
	for (unsigned int i = 0; i < len; i++)
		usart1_send_char(*ptr++);

	return len;
}

int _close(int file)
{
	return -1;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

int _read(int file, char *ptr, int len)
{
	return 0;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	return 1;
}