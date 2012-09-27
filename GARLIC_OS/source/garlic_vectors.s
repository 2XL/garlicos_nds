@;==============================================================================
@;
@;	"garlic_vector.s":	vector de direcciones de funciones del API de GARLIC
@;
@;==============================================================================

.section .vectors,"a",%progbits

@;Vector de manejadores de excepciones ARM
@;ATENCIÓN: ¡NO funciona en un entorno emulado (DeSmuME)!
@;Por este motivo, el siguiente código está comentado:
@;
@;	.arm
@;SystemVectors:		@; Vector de excepciones ARM
@;	ldr	pc, bios_reset
@;	ldr	pc, bios_undefined
@;	b	garlic_SWI				@; en vez de 'ldr pc, =bios_swi'
@;	ldr	pc, bios_prefetch_abort
@;	ldr	pc, bios_data_abort
@;dummy:	b	dummy
@;	ldr	pc, bios_irq
@;	ldr	pc, bios_fiq
@;
@;						@; Direcciones de manejadores de excepciones de la BIOS
@;bios_reset:
@;	.word	0xFFFF0000
@;bios_undefined:
@;	.word	0xFFFF0004
@;bios_swi:
@;	.word	0xFFFF0008
@;bios_prefetch_abort:
@;	.word	0xFFFF000C
@;bios_data_abort:
@;	.word	0xFFFF0010
@;bios_irq:
@;	.word	0xFFFF0018
@;bios_fiq:
@;	.word	0xFFFF001C
@;
@;
@;	Nueva rutina de tratamiento de excepciones SWI, para llamadas al sistema
@;	GARLIC (con identificador de SWI mayor o igual a 0x20)
@;garlic_SWI:
@;		push {r4, r5}			@; guardar registros modificados en pila de SVC
@;		
@;		mov r4, lr
@;		sub r4, #2
@;		ldrb r5, [r4]			@; R5 = código identificador de SWI (byte)
@;		cmp r5, #0x20
@;		bhs .LGarlicFunc		@; si >= 0x20, se trata de una llamada GARLIC
@;								@; sinó, transferir el control a la BIOS
@;		pop {r4, r5}			@; restaurar registros
@;		ldr pc, =bios_swi		@; saltar al tratamiento de SWI de la BIOS
@;		
@;	.LGarlicFunc:				@; tratamiento de funciones del API de GARLIC
@;		sub r5, #0x20			@; restar el desplazamiento al número de func.
@;		ldr r4, =APIVector		@; R4 = base de vector de direcciones de func.
@;		bl [r4, r5, lsl #2]		@; salto indirecto a través del vector
@;		
@;		pop {r4, r5}			@; recuperar registros modificados
@;		movs pc, lr				@; retorno de la excepción SWI
@;
@;	.pool						@; posición para guardar valores de 32 bits
@;
@;¡FIN del código comentado!


APIVector:						@; Vector de direcciones de funciones
	.word	_gi_random			@; (código de funciones en "garlic_itcm_api.s")
	.word	_gi_divmod
	.word	_gi_num2str
	.word	_gi_print

.end