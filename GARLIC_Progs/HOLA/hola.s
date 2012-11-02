	.arch armv5te
	.fpu softvfp
	.eabi_attribute 23, 1
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"HOLA.c"
	.section	.rodata
	.align	2
.LC0:
	.ascii	"-- Programa HOLA (Garlic 1.0) --\000"
	.align	2
.LC1:
	.ascii	"\011Hello world!\012\000"
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
	sub	sp, sp, #28
	str	r0, [sp, #4]
	ldr	r0, .L8
	mov	r1, #2
	bl	GARLIC_print
	mov	r3, #1
	str	r3, [sp, #16]
	ldr	r3, [sp, #4]
	cmp	r3, #3
	ble	.L2
	mov	r3, #3
	str	r3, [sp, #4]
.L2:
	mov	r3, #0
	str	r3, [sp, #20]
	b	.L3
.L4:
	ldr	r2, [sp, #16]
	mov	r3, r2
	mov	r3, r3, asl #2
	add	r3, r3, r2
	mov	r3, r3, asl #1
	str	r3, [sp, #16]
	ldr	r3, [sp, #20]
	add	r3, r3, #1
	str	r3, [sp, #20]
.L3:
	ldr	r2, [sp, #4]
	ldr	r3, [sp, #20]
	cmp	r2, r3
	bhi	.L4
	mov	r3, #0
	str	r3, [sp, #20]
	b	.L5
.L6:
	add	r3, sp, #8
	mov	r0, r3
	mov	r1, #5
	ldr	r2, [sp, #20]
	bl	GARLIC_num2str
	add	r3, sp, #8
	mov	r0, r3
	mov	r1, #1
	bl	GARLIC_print
	ldr	r0, .L8+4
	mov	r1, #0
	bl	GARLIC_print
	ldr	r3, [sp, #20]
	add	r3, r3, #1
	str	r3, [sp, #20]
.L5:
	ldr	r2, [sp, #20]
	ldr	r3, [sp, #16]
	cmp	r2, r3
	bcc	.L6
	mov	r3, #0
	mov	r0, r3
	add	sp, sp, #28
	ldmfd	sp!, {pc}
.L9:
	.align	2
.L8:
	.word	.LC0
	.word	.LC1
	.size	_start, .-_start
	.ident	"GCC: (devkitARM release 41) 4.7.1"
