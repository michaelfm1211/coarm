.section ".text.boot"

.globl _start
_start:
	// setup the stack at 1kb less than the interrupt stack.
	mov sp, #0x7C00

	// run the kmain in an infinite loop
	// r0 has a firmware code, r1 has the machine ID, and r2 has ATAGS
	bl kmain
	b _start
