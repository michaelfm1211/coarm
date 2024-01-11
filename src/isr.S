.section .text

.globl isr_undefined
isr_undefined:
	mov sp, 0x8000
	bl isr_undefined_handler
	// no mercy, restart kmain
	b _start

.globl isr_software
isr_software:
	mov sp, 0x8000
	stmfd sp!, {r0-r12, lr}
	mov r0, lr
	mov r1, sp
	bl isr_software_handler
	ldmfd sp!, {r0-r12, pc}^

.globl isr_prefetch
isr_prefetch:
	mov sp, 0x8000
	bl isr_prefetch_handler
	// no mercy, restart kmain
	b _start

.globl isr_data
isr_data:
	mov sp, 0x8000
	bl isr_data_handler
	// no mercy, restart kmain
	b _start
