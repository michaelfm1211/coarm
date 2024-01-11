#pragma once

#include <stdint.h>

// function select
#define GPFSEL0 (volatile uint32_t *)0x20200000
#define GPFSEL1 (volatile uint32_t *)0x20200004
#define GPFSEL2 (volatile uint32_t *)0x20200008
#define GPFSEL3 (volatile uint32_t *)0x2020000C
#define GPFSEL4 (volatile uint32_t *)0x20200010
#define GPFSEL5 (volatile uint32_t *)0x20200014

#define GPIO_ALT_3 0b011
#define GPIO_ALT_5 0b010

// pin output bit field
#define GPSET0 (volatile uint32_t *)0x2020001C
#define GPSET1 (volatile uint32_t *)0x20200020

// pin clear bit field
#define GPCLR0 (volatile uint32_t *)0x20200028
#define GPCLR1 (volatile uint32_t *)0x2020002C

// pin level bit field
#define GPLEV0 (volatile uint32_t *)0x20200034
#define GPLEV1 (volatile uint32_t *)0x20200038

// pin event detect status bit field
#define GPEDS0 (volatile uint32_t *)0x20200040
#define GPEDS1 (volatile uint32_t *)0x20200044

// pin rising edge detect enable bit field
#define GPREN0 (volatile uint32_t *)0x2020004C
#define GPREN1 (volatile uint32_t *)0x20200050

// pin falling edge detect enable bit field
#define GPFEN0 (volatile uint32_t *)0x20200058
#define GPFEN1 (volatile uint32_t *)0x2020005C

// pin high detect enable bit field
#define GPHEN0 (volatile uint32_t *)0x20200064
#define GPHEN1 (volatile uint32_t *)0x20200068

// pin low detect enable bit field
#define GPLEN0 (volatile uint32_t *)0x20200070
#define GPLEN1 (volatile uint32_t *)0x20200074

// pin async rising edge detect enable bit field
#define GPAREN0 (volatile uint32_t *)0x2020007C
#define GPAREN1 (volatile uint32_t *)0x20200080

// pin async falling edge detect enable bit field
#define GPAFEN0 (volatile uint32_t *)0x20200088
#define GPAFEN1 (volatile uint32_t *)0x2020008C

// pin pull-up/down enable bit field
#define GPPUD (volatile uint32_t *)0x20200094

// pin pull-up/down enable clock bit field
#define GPPUDCLK0 (volatile uint32_t *)0x20200098
#define GPPUDCLK1 (volatile uint32_t *)0x2020009C

