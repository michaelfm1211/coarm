CC = clang
LD = ld.lld

CFLAGS = -ffreestanding -mcpu=arm1176jzf-s --target=arm-unknown-none-gnueabihi

all: test.bin

test.o: test.S
	$(CC) $(CFLAGS) -c -o $@ $^

test.bin: test.o
	$(LD) --oformat binary -o $@ $^

.PHONY: clean
clean:
	rm -rf test.o test.bin
