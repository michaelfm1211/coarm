#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <termios.h>
#include <unistd.h>

#define PROTO_VERSION "COARM000"

int set_tty_attrs(int serial) {
  // get the terminal settings
  struct termios tty;
  if (tcgetattr(serial, &tty) < 0) {
    perror("tcgetattr()");
    return -1;
  }

  // set baud rate to 115200
  if (cfsetispeed(&tty, B115200) < 0) {
    perror("cfsetispeed()");
    return -1;
  }
  if (cfsetospeed(&tty, B115200) < 0) {
    perror("cfsetospeed()");
    return -1;
  }

  // raw terminal, no parity, block on read until data is ready, no time limit
  cfmakeraw(&tty);
  tty.c_cflag &= ~CSTOPB;
  tty.c_cc[VMIN] = 1;
  tty.c_cc[VTIME] = 0;

  // write terminal settings
  if (tcsetattr(serial, TCSANOW, &tty) < 0) {
    perror("tcsetattr()");
    return -1;
  }

  return 0;
}

int block_read(int fd, char *buf, int bytes) {
#ifdef DEBUG
  fprintf(stderr, "attempting to read %d bytes\n", bytes);
#endif
  while (bytes) {
    int len = read(fd, buf, bytes);
    if (len < 0) {
      perror("read");
      return -1;
    }
#ifdef DEBUG
    fprintf(stderr, "\tread %d bytes\n", len);
    for (int i = 0; i < len; i++) {
      fprintf(stderr, "\t\t%02x\n", *((uint8_t *)buf + i));
    }
#endif
    bytes -= len;
    buf += len;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  char buf[1024] = {0};
  int writelen = 0;

  // usage
  if (argc != 2) {
    fprintf(stderr, "%s devfile < code.o > out.bin\n", argv[0]);
    return 1;
  }

  // open the device file
  int serial = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC);
  if (serial < 0) {
    perror("open()");
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "opened device %s\n", argv[1]);
#endif

  // setup the serial port's settings
  if (set_tty_attrs(serial) < 0) {
    close(serial);
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "configured tty\n");
#endif

  // send ready signal
  *buf = 'R';
  if ((writelen = write(serial, buf, 1)) < 0) {
    perror("write()");
    close(serial);
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "sent ready signal (%d bytes)\n", writelen);
#endif

  // read the version string and UART implementation addresses
  if (block_read(serial, buf, 16) < 0) {
    close(serial);
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "received version string and UART implementation\n");
  fprintf(stderr, "\tversion: %.8s\n", buf);
  fprintf(stderr, "\tuart_rx(): 0x%x\n", *(uint32_t *)(buf + 8));
  fprintf(stderr, "\tuart_tx(): 0x%x\n", *(uint32_t *)(buf + 12));
#endif
  if (strncmp(buf, PROTO_VERSION, 8) != 0) {
    fprintf(stderr, "error: protocol version is not " PROTO_VERSION "\n");
    close(serial);
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "device supports protocol version " PROTO_VERSION "\n");
#endif

  // copy stdin to the serial connection, then send EOT
  int readlen = 0;
  while ((readlen = read(0, buf, 1024)) > 0) {
#ifdef DEBUG
    fprintf(stderr, "read %d bytes of code from stdin...\n", readlen);
#endif
    if ((writelen = write(serial, buf, readlen)) < 0) {
      perror("write()");
      close(serial);
      return 1;
    }
#ifdef DEBUG
    fprintf(stderr, "...sent %d bytes of code\n", writelen);
#endif
  }
  if (readlen < 0) {
    perror("read()");
    close(serial);
    return 1;
  }
  strncpy(buf, "\x04\x04\x04\x04", 4);
  if ((writelen = write(serial, buf, 4)) < 0) {
    perror("write()");
    close(serial);
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "sent end of transmission (%d bytes)\n", writelen);
#endif

  // read the EOT and return code from the serial connection. we don't have to
  // worry about anything else because we didn't give the user code access to
  // the UART implementations.
  if (block_read(serial, buf, 8) < 0) {
    close(serial);
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "received end of transmission and return value\n");
  fprintf(stderr, "\t%02x %02x %02x %02x\n", *(uint8_t *)buf,
          *((uint8_t *)buf + 1), *((uint8_t *)buf + 2), *((uint8_t *)buf + 3));
  fprintf(stderr, "\t%02x %02x %02x %02x\n", *((uint8_t *)buf + 4),
          *((uint8_t *)buf + 5), *((uint8_t *)buf + 6), *((uint8_t *)buf + 7));
#endif

  close(serial);

  uint32_t retval = *(buf + 4);
#ifdef DEBUG
  fprintf(stderr, "return value: %u\n", retval);
#endif
  return retval;
}
