#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <termios.h>

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
  if (cfsetospeed(&tty, 0) < 0) {
    perror("cfsetospeed()");
    return -1;
  }

  // no break processing or flow control
  tty.c_iflag &= ~IGNBRK;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  
  // no remapping or delays
  tty.c_oflag = 0;

  // 8N1, no control, enable receiver
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
  tty.c_cflag &= ~(CSTOPB);
  tty.c_cflag &= ~(PARENB | PARODD);
  tty.c_cflag &= CLOCAL;
  tty.c_cflag &= CREAD;

  // disable signals, canonical mode, echo
  tty.c_lflag = 0;

  // no minimum number of bytes for read, set timeout for read to .5 seconds
  tty.c_cc[VMIN] = 255;
  tty.c_cc[VTIME] = 5;

  // write terminal settings
  if (tcsetattr(serial, TCSANOW, &tty) < 0) {
    perror("tcsetattr()");
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  char buf[1024] = {0};
  int readlen = 0;

  // usage
  if (argc != 2) {
    fprintf(stderr, "%s devfile < code.o > out.bin\n", argv[0]);
    return 1;
  }

  // open the device file
  int serial = open(argv[1], O_RDWR | O_EXLOCK);
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

  // send ready signal
  *buf = 'R';
  if (write(serial, buf, 1) < 0) {
    perror("write()");
    close(serial);
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "sent ready signal\n");
#endif

  // read the version string and UART implementation addresses
  if ((readlen = read(serial, buf, 16)) < 0) {
    perror("read()");
    close(serial);
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "received version string and UART implementation\n");
  fprintf(stderr, "\tgot %d bytes\n", readlen);
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
  while ((readlen = read(0, buf, 1024)) > 0) {
    if (write(serial, buf, readlen) < 0) {
      perror("write()");
      close(serial);
      return 1;
    }
  }
  if (readlen < 0) {
    perror("read()");
    close(serial);
    return 1;
  }
  strncpy(buf, "\x04\x04\x04\x04", 4);
  if (write(serial, buf, 4) < 0) {
    perror("write()");
    close(serial);
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "finished sending code to device\n");
#endif

  // read the EOT and return code from the serial connection. we don't have to
  // worry about anything else because we didn't give the user code access to
  // the UART implementations.
  if (read(serial, buf, 8) < 0) {
    perror("read()");
    close(serial);
    return 1;
  }
#ifdef DEBUG
  fprintf(stderr, "read end of transmission and return value\n");
#endif

  close(serial);

  uint32_t retval = *(buf + 4);
#ifdef DEBUG
  fprintf(stderr, "return value: %u\n", retval);
#endif
  return retval;
}
