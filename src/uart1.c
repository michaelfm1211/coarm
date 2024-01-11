#include "uart1.h"
#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>

static void wait_150_cycles(void) {
  uint32_t r = 150;
  while (r--) {
    __asm__ __volatile__ ("");
  }
}

void uart1_init(void) {
  // setup UART1
  *AUX_ENABLES |= 1; // enable UART1, AUX mini UART
  *AUX_MU_CNTL_REG = 0;
  // see https://github.com/dwelch67/raspberrypi-zero/tree/master/uart01
  // for why AUX_MU_LCR_REG is set to 3. The documentation is wrong.
  *AUX_MU_LCR_REG = 3;    // turn on 8-bit mode
  *AUX_MU_MCR_REG = 0;    // no flow control
  *AUX_MU_IIR_REG = 0;    // turn off interrupts
  *AUX_MU_IER_REG = 0xc6; // clear and enable FIFOs
  *AUX_MU_BAUD_REG = 270; // 115200 baud

  // map UART1 to GPIO pins
  uint32_t r = *GPFSEL1;
  r &= ~((7 << 12) | (7 << 15)); // GPIO 14 and 15
  r |= (2 << 12) | (2 << 15);    // ALT5
  *GPFSEL1 = r;

  *GPPUD = 0;
  wait_150_cycles();
  *GPPUDCLK0 = (1 << 14) | (1 << 15);
  wait_150_cycles();
  *GPPUDCLK0 = 0;

  // clear the FIFO
  while (*AUX_MU_LSR_REG & 0x01)
    (void)*AUX_MU_IO_REG;

  // enable TX and RX
  *AUX_MU_CNTL_REG = 3;
}

void uart1_tx(uint8_t ch) {
  while (!(*AUX_MU_LSR_REG & 0x20)) {
  }
  *AUX_MU_IO_REG = ch;
}

uint8_t uart1_rx(void) {
  while (!(*AUX_MU_LSR_REG & 0x01)) {
  }
  return *AUX_MU_IO_REG;
}
