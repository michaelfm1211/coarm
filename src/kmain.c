#include "src/isr.h"
#include "uart1.h"
#include <stdbool.h>
#include <stdint.h>

extern uint8_t __end;

void wait_for_ready(void) {
  uint8_t ch = 0;
  while (ch != 'R') {
    ch = uart1_rx();
  }
}

// send the COARM header over UART1
void send_hdr(void) {
  // Send the magic two words
  const char *hdr = "COARM000";
  while (*hdr) {
    uart1_tx(*hdr);
    hdr++;
  }

  // Send the address of the UART receive and trasmit functions in little endian
  uint32_t rx_addr = (uint32_t)uart1_rx;
  for (int i = 0; i < 4; i++) {
    uart1_tx(rx_addr >> (8 * i));
  }
  uint32_t tx_addr = (uint32_t)uart1_tx;
  for (int i = 0; i < 4; i++) {
    uart1_tx(tx_addr >> (8 * i));
  }
}

// receive code over UART1
void recv_code(uint8_t *buf) {
  send_hdr();

  int escape_cnt = 0;
  while (escape_cnt != 4) {
    uint8_t byte = uart1_rx();
    if (byte == 0x04) {
      // 0x04 is the escape byte. four escape bytes signal EOT
      escape_cnt++;
      continue;
    }
    if (escape_cnt != 0) {
      // if we have less than four escape bytes, then this is not an EOT and we
      // should make up for whatever we upheld.
      for (int i = 0; i < escape_cnt; i++) {
        *buf = 0x04;
        buf++;
      }
      escape_cnt = 0;
    }
    // write received bytes to the buffer
    *buf = byte;
    buf++;
  }

  // add an extra "bx lr" instruction to return from the user code (little
  // endian) and to make sure we don't run any previously received code still
  // in memory.
  *buf++ = 0x1e;
  *buf++ = 0xff;
  *buf++ = 0x2f;
  *buf++ = 0xe1;
}

void send_eot(uint32_t retval) {
  // send 4 escape bytes signalling EOT
  for (int i = 0; i < 4; i++) {
    uart1_tx(0x04);
  }
  // finish by sending the one word return code
  for (int i = 0; i < 4; i++) {
    uart1_tx(retval >> (8 * i));
  }
}

// NOTE: This is not your regular kmain()! boot.S runs this function in a loop
// forever.
void kmain(void) {
  uart1_init();
  isr_init();

  // wait until client sends a ready signal, then receive code
  wait_for_ready();
  recv_code(&__end);

  // return value is in r0, so give r0 a default value in case user code
  // doesn't use it
  __asm__ __volatile__("mov r0, #0");
  int32_t retval = ((int32_t(*)(void)) & __end)();

  // send return value in little endian
  send_eot(retval);
}
