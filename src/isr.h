#pragma once

#include <stdint.h>

// These are the addresses the CPU will jump to on an interrupt. Don't use these
// for adding handlers. Put those in the HANDLER table.
#define EXCEPTION_UNDEFINED (uint32_t *)0x4
#define EXCEPTION_SOFTWARE (uint32_t *)0x8
#define EXCEPTION_PREFETCH (uint32_t *)0xC
#define EXCEPTION_DATA (uint32_t *)0x10

// These are the pointers to the addresses that will be jumped to by the
// vectors. These are the addresses you should use for adding handlers.
#define HANDLER_UNDEFINED (uint32_t *)0x24
#define HANDLER_SOFTWARE (uint32_t *)0x28
#define HANDLER_PREFETCH (uint32_t *)0x2C
#define HANDLER_DATA (uint32_t *)0x30

void isr_init(void);
