#pragma once

#include <stdint.h>

#define AUX_ENABLES (volatile uint8_t *)0x20215004
#define AUX_MU_IO_REG (volatile uint8_t *)0x20215040
#define AUX_MU_IIR_REG (volatile uint8_t *)0x20215044
#define AUX_MU_IER_REG (volatile uint8_t *)0x20215048
#define AUX_MU_LCR_REG (volatile uint8_t *)0x2021504C
#define AUX_MU_MCR_REG (volatile uint8_t *)0x20215050
#define AUX_MU_LSR_REG (volatile uint8_t *)0x20215054
#define AUX_MU_MSR_REG (volatile uint8_t *)0x20215058
#define AUX_MU_SCRATCH (volatile uint8_t *)0x2021505C
#define AUX_MU_CNTL_REG (volatile uint8_t *)0x20215060
#define AUX_MU_STAT_REG (volatile uint8_t *)0x20215064
#define AUX_MU_BAUD_REG (volatile uint16_t *)0x20215068

void uart1_init(void);

void uart1_tx(uint8_t ch);
uint8_t uart1_rx(void);
