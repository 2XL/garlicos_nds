	.arch armv5te
	.fpu softvfp
	.eabi_attribute 23, 1
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.file	"hola.c"
	.section	.rodata
	.align	2
.LC0:
	.ascii	"-- Programa HOLA (Garlic 0.5) --\000"
	.align	2
.LC1:
	.ascii	"\011Hello world!\012\000"
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
	sub	sp, sp, #20
	ldr	r0, .L4
	bl	GARLIC_print
	bl	GARLIC_random
	mov	r3, r0
	mov	r1, r3
	add	r2, sp, #12
	add	r3, sp, #8
	mov	r0, r1
	mov	r1, #100
	bl	GARLIC_divmod
	ldr	r3, [sp, #8]
	add	r3, r3, #1
	str	r3, [sp, #8]
	mov	r3, #0
	str	r3, [sp, #12]
	b	.L2
.L3:
	ldr	r3, [sp, #12]
	mov	r2, sp
	mov	r0, r2
	mov	r1, #5
	mov	r2, r3
	bl	GARLIC_num2str
	mov	r3, sp
	mov	r0, r3
	bl	GARLIC_print
	ldr	r0, .L4+4
	bl	GARLIC_print
	ldr	r3, [sp, #12]
	add	r3, r3, #1
	str	r3, [sp, #12]
.L2:
	ldr	r2, [sp, #12]
	ldr	r3, [sp, #8]
	cmp	r2, r3
	bcc	.L3
	mov	r3, #0
	mov	r0, r3
	add	sp, sp, #20
	ldmfd	sp!, {pc}
.L5:
	.align	2
.L4:
	.word	.LC0
	.word	.LC1
	.size	_start, .-_start
	.ident	"GCC: (devkitARM release 34) 4.6.1"
