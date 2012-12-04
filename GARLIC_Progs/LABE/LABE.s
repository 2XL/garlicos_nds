	.arch armv5te
	.fpu softvfp
	.eabi_attribute 23, 1
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"LABE.c"
	.comm	Maze,1536,4
	.comm	zoo,144,4
	.section	.rodata
	.align	2
.LC0:
	.ascii	"-- Programa LABE (Garlic 1.0) --\000"
	.text
	.align	2
	.global	_start
	.type	_start, %function
_start:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
	sub	sp, sp, #12
	str	r0, [sp, #4]
	bl	GARLIC_clear
	ldr	r0, .L3
	mov	r1, #2
	bl	GARLIC_print
	mov	r0, #2
	bl	_createLaberint
	mov	r3, #0
	mov	r0, r3
	add	sp, sp, #12
	ldmfd	sp!, {pc}
.L4:
	.align	2
.L3:
	.word	.LC0
	.size	_start, .-_start
	.align	2
	.global	_createLaberint
	.type	_createLaberint, %function
_createLaberint:
	@ args = 0, pretend = 0, frame = 56
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
	sub	sp, sp, #60
	str	r0, [sp, #4]
	ldr	r3, [sp, #4]
	add	r3, r3, #1
	str	r3, [sp, #4]
	mov	r3, #16
	str	r3, [sp, #28]
	ldr	r3, [sp, #4]
	mov	r3, r3, asl #3
	str	r3, [sp, #24]
	ldr	r3, [sp, #28]
	ldr	r2, [sp, #24]
	mul	r2, r3, r2
	mov	r3, r2
	mov	r3, r3, asl #2
	add	r3, r3, r2
	mov	r2, r3, asl #2
	add	r3, r3, r2
	ldr	r2, .L20
	smull	r1, r2, r3, r2
	mov	r2, r2, asr #5
	mov	r3, r3, asr #31
	rsb	r3, r3, r2
	str	r3, [sp, #20]
	mov	r3, #0
	str	r3, [sp, #52]
	ldr	r3, [sp, #4]
	and	r3, r3, #3
	str	r3, [sp, #16]
	mov	r3, #0
	str	r3, [sp, #48]
	mov	r3, #0
	str	r3, [sp, #44]
	mov	r3, #0
	str	r3, [sp, #48]
	mov	r3, #0
	str	r3, [sp, #44]
	b	.L6
.L7:
	ldr	r3, [sp, #44]
	add	r3, r3, #4
	mov	r0, #0
	mov	r1, r3
	mov	r2, #95
	ldr	r3, [sp, #16]
	bl	GARLIC_printchar
	ldr	r3, [sp, #24]
	sub	r2, r3, #1
	ldr	r3, [sp, #44]
	add	r3, r3, #4
	mov	r0, r2
	mov	r1, r3
	mov	r2, #95
	ldr	r3, [sp, #16]
	bl	GARLIC_printchar
	ldr	r3, [sp, #48]
	add	r3, r3, #1
	str	r3, [sp, #48]
	ldr	r3, [sp, #44]
	add	r3, r3, #1
	str	r3, [sp, #44]
.L6:
	ldr	r2, [sp, #48]
	ldr	r3, [sp, #28]
	cmp	r2, r3
	blt	.L7
	mov	r3, #0
	str	r3, [sp, #48]
	mov	r3, #0
	str	r3, [sp, #44]
	b	.L8
.L9:
	ldr	r0, [sp, #48]
	mov	r1, #4
	mov	r2, #95
	ldr	r3, [sp, #16]
	bl	GARLIC_printchar
	ldr	r3, [sp, #28]
	add	r3, r3, #3
	ldr	r0, [sp, #48]
	mov	r1, r3
	mov	r2, #95
	ldr	r3, [sp, #16]
	bl	GARLIC_printchar
	ldr	r3, [sp, #44]
	add	r3, r3, #1
	str	r3, [sp, #44]
	ldr	r3, [sp, #48]
	add	r3, r3, #1
	str	r3, [sp, #48]
.L8:
	ldr	r2, [sp, #44]
	ldr	r3, [sp, #24]
	cmp	r2, r3
	blt	.L9
	ldr	r3, [sp, #24]
	ldr	r2, [sp, #4]
	mul	r3, r2, r3
	ldr	r2, [sp, #20]
	rsb	r3, r3, r2
	str	r3, [sp, #20]
	ldr	r3, [sp, #20]
	str	r3, [sp, #52]
	mov	r3, #0
	str	r3, [sp, #48]
	ldr	r3, [sp, #4]
	mov	r3, r3, asl #3
	sub	r3, r3, #2
	str	r3, [sp, #12]
	mov	r3, #14
	str	r3, [sp, #8]
	ldr	r3, [sp, #12]
	ldr	r2, [sp, #8]
	mul	r2, r3, r2
	mov	r3, r2
	mov	r3, r3, asl #2
	add	r3, r3, r2
	mov	r2, r3, asl #2
	add	r3, r3, r2
	str	r3, [sp, #20]
	ldr	r3, [sp, #20]
	ldr	r2, .L20
	smull	r1, r2, r3, r2
	mov	r2, r2, asr #5
	mov	r3, r3, asr #31
	rsb	r3, r3, r2
	str	r3, [sp, #20]
	ldr	r3, [sp, #20]
	str	r3, [sp, #52]
.L14:
	bl	GARLIC_random
	mov	r3, r0
	and	r3, r3, #255
	str	r3, [sp, #40]
	b	.L10
.L11:
	ldr	r2, [sp, #40]
	ldr	r3, [sp, #12]
	rsb	r3, r3, r2
	str	r3, [sp, #40]
.L10:
	ldr	r3, [sp, #12]
	sub	r2, r3, #1
	ldr	r3, [sp, #40]
	cmp	r2, r3
	blt	.L11
	bl	GARLIC_random
	mov	r3, r0
	and	r3, r3, #255
	str	r3, [sp, #36]
	b	.L12
.L13:
	ldr	r2, [sp, #36]
	ldr	r3, [sp, #8]
	rsb	r3, r3, r2
	str	r3, [sp, #36]
.L12:
	ldr	r3, [sp, #8]
	sub	r2, r3, #1
	ldr	r3, [sp, #36]
	cmp	r2, r3
	blt	.L13
	ldr	r3, [sp, #36]
	add	r3, r3, #4
	ldr	r0, [sp, #40]
	mov	r1, r3
	mov	r2, #14
	ldr	r3, [sp, #16]
	bl	GARLIC_printchar
	ldr	r3, [sp, #52]
	sub	r3, r3, #1
	str	r3, [sp, #52]
	ldr	r3, [sp, #52]
	cmp	r3, #0
	bgt	.L14
	mov	r3, #0
	str	r3, [sp, #32]
.L19:
	bl	GARLIC_random
	mov	r3, r0
	and	r3, r3, #255
	str	r3, [sp, #40]
	b	.L15
.L16:
	ldr	r2, [sp, #40]
	ldr	r3, [sp, #12]
	rsb	r3, r3, r2
	str	r3, [sp, #40]
.L15:
	ldr	r2, [sp, #40]
	ldr	r3, [sp, #12]
	cmp	r2, r3
	bgt	.L16
	bl	GARLIC_random
	mov	r3, r0
	and	r3, r3, #255
	str	r3, [sp, #36]
	b	.L17
.L18:
	ldr	r2, [sp, #36]
	ldr	r3, [sp, #8]
	rsb	r3, r3, r2
	str	r3, [sp, #36]
.L17:
	ldr	r2, [sp, #36]
	ldr	r3, [sp, #8]
	cmp	r2, r3
	bgt	.L18
	ldr	r3, [sp, #40]
	add	r1, r3, #1
	ldr	r3, [sp, #36]
	add	r2, r3, #5
	ldr	r3, [sp, #32]
	and	r3, r3, #255
	add	r3, r3, #33
	and	r3, r3, #255
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3
	ldr	r3, [sp, #32]
	bl	GARLIC_printchar
	ldr	r3, [sp, #32]
	add	r3, r3, #1
	str	r3, [sp, #32]
	ldr	r2, [sp, #32]
	mov	r3, r2
	mov	r3, r3, asl #3
	rsb	r3, r2, r3
	add	r2, r3, #4
	ldr	r3, [sp, #32]
	and	r3, r3, #255
	add	r3, r3, #33
	and	r3, r3, #255
	mov	r0, r2
	mov	r1, #21
	mov	r2, r3
	ldr	r3, [sp, #32]
	bl	GARLIC_printchar
	ldr	r2, [sp, #32]
	mov	r3, r2
	mov	r3, r3, asl #3
	rsb	r3, r2, r3
	add	r3, r3, #5
	mov	r0, r3
	mov	r1, #21
	mov	r2, #26
	ldr	r3, [sp, #32]
	bl	GARLIC_printchar
	ldr	r2, [sp, #4]
	ldr	r3, [sp, #32]
	cmp	r2, r3
	bgt	.L19
	add	sp, sp, #60
	ldmfd	sp!, {pc}
.L21:
	.align	2
.L20:
	.word	1374389535
	.size	_createLaberint, .-_createLaberint
	.ident	"GCC: (devkitARM release 41) 4.7.1"
