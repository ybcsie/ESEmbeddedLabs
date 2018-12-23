#ifndef CLOCK_H
#define CLOCK_H
#include <stdint.h>

void op_sysclk(uint8_t div);
void set_sysclk_pll(void);

#endif