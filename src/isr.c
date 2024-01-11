#include <stdint.h>
#include "isr.h"
#include "uart1.h"

// adapted from https://developer.arm.com/documentation/dui0056/d/handling-processor-exceptions/installing-an-exception-handler/installing-the-handlers-from-c?lang=en
static void install_vector(uint32_t address, volatile uint32_t *vector) {
  *vector = 0xE59FF000 | (address - (uint32_t)vector - 0x8);
}

// ISRs implemented in kernel/isr.S call the C handlers below
extern void isr_undefined(void);
extern void isr_software(void);
extern void isr_prefetch(void);
extern void isr_data(void);

void isr_undefined_handler(void) {
  for (int i = 0; i < 4; i++) {
    uart1_tx(0x04);
  }
  uart1_tx('E');
  uart1_tx('U');
  uart1_tx(0);
  uart1_tx(0);
}

// WARNING: Do not use float registers here! The ISR stub doesn't currently save
// them so you could clobber the interruptee's float registers. This is only a
// concern for this function because none of the other ISRs are supposed to
// return nicely.
void isr_software_handler(__attribute__((unused)) uint32_t lr, __attribute__((unused)) uint32_t sp) {
  /* uint32_t code = *(uint32_t *)(lr - 0x4) & 0x00FFFFFF; */
  // intentionally unimplemented
}

void isr_prefetch_handler(void) {
  for (int i = 0; i < 4; i++) {
    uart1_tx(0x04);
  }
  uart1_tx('E');
  uart1_tx('P');
  uart1_tx(0);
  uart1_tx(0);
}

void isr_data_handler(void) {
  for (int i = 0; i < 4; i++) {
    uart1_tx(0x04);
  }
  uart1_tx('E');
  uart1_tx('D');
  uart1_tx(0);
  uart1_tx(0);
}

void isr_init(void) {
  // fill out the handler table
  *HANDLER_UNDEFINED = (uint32_t)isr_undefined;
  *HANDLER_SOFTWARE = (uint32_t)isr_software;
  *HANDLER_PREFETCH = (uint32_t)isr_prefetch;
  *HANDLER_DATA = (uint32_t)isr_data;

  // load the exception vector table with LoaD Relative instructions to the
  // handler table.
  install_vector((uint32_t)HANDLER_UNDEFINED, EXCEPTION_UNDEFINED);
  install_vector((uint32_t)HANDLER_SOFTWARE, EXCEPTION_SOFTWARE);
  install_vector((uint32_t)HANDLER_PREFETCH, EXCEPTION_PREFETCH);
  install_vector((uint32_t)HANDLER_DATA, EXCEPTION_DATA);
}

