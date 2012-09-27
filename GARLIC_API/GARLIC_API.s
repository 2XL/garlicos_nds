@;==============================================================================
@;
@;	"GARLIC_API.s":	implementaci�n de funciones del API del sistema operativo
@;					GARLIC (descripci�n de funciones en "GARLIC_API.h")
@;
@;==============================================================================

.text
	.arm
	.align 2

	.global GARLIC_random
GARLIC_random:
	push {r4, lr}
	mov r4, #0				@; vector base de funciones GARLIC 
	mov lr, pc				@; guardar direcci�n de retorno
	ldr pc, [r4]			@; llamada indirecta a la funci�n 0x00 de GARLIC
	pop {r4, pc}

	.global GARLIC_divmod
GARLIC_divmod:
	push {r4, lr}
	mov r4, #0
	mov lr, pc
	ldr pc, [r4, #4]		@; llamada indirecta a la funci�n 0x01
	pop {r4, pc}

	.global GARLIC_num2str
GARLIC_num2str:
	push {r4, lr}
	mov r4, #0
	mov lr, pc
	ldr pc, [r4, #8]		@; llamada indirecta a la funci�n 0x02
	pop {r4, pc}

	.global GARLIC_print
GARLIC_print:
	push {r4, lr}
	mov r4, #0
	mov lr, pc
	ldr pc, [r4, #12]		@; llamada indirecta a la funci�n 0x03
	pop {r4, pc}

.end
