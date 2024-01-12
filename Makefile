CC = clang
LD = ld.lld
OBJCOPY = llvm-objcopy

TARGET = arm-unknown-none-gnueabihi
CFLAGS = --target=$(TARGET) -I . -Wall -Wextra -Werror -pedantic -mcpu=arm1176jzf-s -ffreestanding -c
LDFLAGS = -m armelf -T linker.ld -nostdlib

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o) src/boot.o src/isrS.o

TOOLS_CFLAGS = -O2 -Wall -Wextra -Werror -pedantic
TOOLS = coarm-send

.PHONY: all
all: CFLAGS += -O2
all: kernel.img $(TOOLS)

.PHONY: debug
debug: CFLAGS += -O0 -g -DDEBUG
debug: TOOLS_CFLAGS += -g -fsanitize=address -fsanitize=undefined -DDEBUG
debug: kernel.img $(TOOLS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

src/boot.o: src/boot.S
	$(CC) $(CFLAGS) -o $@ $^

src/isrS.o: src/isr.S
	$(CC) $(CFLAGS) -o $@ $^

kernel.elf: $(OBJS) src/boot.o
	$(LD) $(LDFLAGS) -o kernel.elf $^

kernel.img: kernel.elf
	$(OBJCOPY) kernel.elf -O binary kernel.img

coarm-send: tools/coarm-send.c
	$(CC) $(TOOLS_CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -rf $(OBJS) src/boot.o kernel.elf kernel.img $(TOOLS) *.dSYM

.PHONY: qemu
qemu: kernel.img
	qemu-system-arm -M raspi0 -kernel kernel.elf -serial null -serial pty
