	.arch armv5te
	.fpu softvfp
	.eabi_attribute 23, 1
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"PONG.c"
	.section	.rodata
	.align	2
.LC0:
	.ascii	"-- Programa PONG (Garlic 1.0) --\000"
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
	sub	sp, sp, #36
	str	r0, [sp, #4]
	bl	GARLIC_clear
	ldr	r0, .L10
	mov	r1, #2
	bl	GARLIC_print
	ldr	r3, [sp, #4]
	cmp	r3, #6
	ble	.L2
	mov	r3, #6
	str	r3, [sp, #4]
.L2:
	ldr	r3, [sp, #4]
	and	r3, r3, #3
	str	r3, [sp, #12]
	mov	r3, #0
	str	r3, [sp, #28]
	mov	r3, #0
	str	r3, [sp, #24]
	mov	r3, #1
	str	r3, [sp, #20]
	mov	r3, #1
	str	r3, [sp, #16]
	ldr	r0, [sp, #28]
	ldr	r1, [sp, #24]
	mov	r2, #95
	ldr	r3, [sp, #12]
	bl	GARLIC_printchar
.L9:
	ldr	r3, [sp, #4]
	mov	r0, r3
	bl	GARLIC_delay
	ldr	r0, [sp, #28]
	ldr	r1, [sp, #24]
	mov	r2, #0
	ldr	r3, [sp, #12]
	bl	GARLIC_printchar
	ldr	r2, [sp, #28]
	ldr	r3, [sp, #20]
	add	r3, r2, r3
	str	r3, [sp, #28]
	ldr	r2, [sp, #24]
	ldr	r3, [sp, #16]
	add	r3, r2, r3
	str	r3, [sp, #24]
	ldr	r3, [sp, #28]
	cmp	r3, #31
	beq	.L3
	ldr	r3, [sp, #28]
	cmp	r3, #0
	bne	.L4
.L3:
	ldr	r3, [sp, #20]
	rsb	r3, r3, #0
	str	r3, [sp, #20]
.L4:
	ldr	r3, [sp, #24]
	cmp	r3, #23
	beq	.L5
	ldr	r3, [sp, #24]
	cmp	r3, #0
	bne	.L6
.L5:
	ldr	r3, [sp, #16]
	rsb	r3, r3, #0
	str	r3, [sp, #16]
.L6:
	ldr	r3, [sp, #28]
	cmp	r3, #0
	bne	.L7
	ldr	r3, [sp, #24]
	cmp	r3, #0
	bne	.L7
	mov	r3, #1
	str	r3, [sp, #24]
	b	.L8
.L7:
	ldr	r3, [sp, #28]
	cmp	r3, #0
	bne	.L8
	ldr	r3, [sp, #24]
	cmp	r3, #1
	bne	.L8
	mov	r3, #0
	str	r3, [sp, #24]
	mov	r3, #1
	str	r3, [sp, #16]
.L8:
	ldr	r0, [sp, #28]
	ldr	r1, [sp, #24]
	mov	r2, #95
	ldr	r3, [sp, #12]
	bl	GARLIC_printchar
	b	.L9
.L11:
	.align	2
.L10:
	.word	.LC0
	.size	_start, .-_start
	.ident	"GCC: (devkitARM release 41) 4.7.1"
