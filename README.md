# coarm

`coarm` lets you send arbitrary and execute arbitrary ARM machine code to a
Raspberry Pi Zero (first generation) over a serial connection. `coarm` runs
bare-metal (ie, without any other operating system), so your machine code has
unlimited access to any peripherals or CPU state.

### Installing

To run `coarm`, you need a FAT formatted SD card with the [proprietary
Raspberry Pi firmware](https://github.com/raspberrypi/firmware) installed. Then,
build `coarm` by running `make` in the source tree and copy the compiled
`kernel.img` binary onto the SD card. You will also need to copy the
`config.txt` file from the source tree onto the SD card.

If you have an SD card that was previously used to run Linux or some other
operating system on a Raspberry Pi, then you likely already have the firmware
installed and you can skip to compiling and copying `kernel.img` and
`config.txt` onto your SD card. If these files already exist on your SD card,
you likely want to make backups of them before you overwrite them.

### Running
Currently, `coarm` requires code to be sent in a raw binary format. The
Raspberry Pi will load your code into memory and calls it. Once your code
finishes by executing a `bx lr` instruction, execution reaches the end of the
received code, or an exception is raised, `coarm` will send back the return
value (whatever is in `r0` on success, or a 4 byte error code beginning with
`'E'`).

The easiest way to get code running is to use `clang` as an assembler to
generate a `.o` object file, use `ld.lld` with the `--oformat binary` option to
convert the object file into a raw binary format, then use the `coarm-send`
client tool provided by this project to send the raw binary.

An example of using `clang`/`ld.lld` to create a raw binary output is in the
`example` subdirectory. Once you have a raw binary, you can use the `coarm-send`
like below to send it to the Raspberry Pi. The example below assumes
`/dev/ttys001` is the serial device of your Raspberry Pi and `code.bin` is your
raw binary:
```
coarm-send /dev/ttys001 < code.bin
```
This command will exit with the value returned by the ARM code. If you're using
the code in the `example` subdirectory, then it should return `11`. If you
compiled `coarm` with debug, then `coarm-send` will output verbose logging
information.
