@;==============================================================================
@;
@;	"garlic_itcm_api.s":	código de las funciones del API de GARLIC (ver
@;						"GARLIC_API.h" para descripción de las funciones)
@;
@;==============================================================================

.section .itcm,"ax",%progbits

	.arm
	.align 2

	.global _gi_random
	@;Resultado:
	@; R0 = valor aleatorio de 32 bits
_gi_random:
		push {r1-r5, lr}
		ldr r0, =_gd_seed
		ldr r1, [r0]			@; R1 = valor de semilla de números aleatorios
		ldr r2, =0x0019660D
		ldr r3, =0x3C6EF35F
		umull r4, r5, r1, r2	@; R5:R4 = _gd_seed * 0x19660D
		add r4, r3				@; R4 += 0x3C6EF35F
		str r4, [r0]			@; guarda la nueva semilla (R4)
		mov r0, r5				@; devuelve por R0 el valor aleatorio (R5)
		pop {r1-r5, pc}


	.global _gi_divmod
	@;Parámetros
	@; R0: unsigned int num,
	@; R1: unsigned int den,
	@; R2: unsigned int * quo,
	@; R3: unsigned int * mod
	@;Resultado
	@; R0: 0 si no hay problema, !=0 si hay error en la división
_gi_divmod:
	@;Código para realizar la división con el hardware matemático de la NDS
	@;ATENCIÓN: ¡NO funciona en un entorno emulado (DeSmuME)!
	@;Por este motivo, el siguiente código está comentado:
@;	push {r4, r5, lr}
@;	mov r4, #0x04000000
@;	add r4, #0x280			@; R4 = dir. base registro DIVCNT (0x04000280)
@;	mov r5, #0
@;	strh r5, [r4]			@; fijar modo 0 (división 32 bits)
@;	str r0, [r4, #0x10]		@; guardar numerador en DIV_NUMER (0x04000290)
@;	str r1, [r4, #0x18]		@; guardar denominador en DIV_DENOM (0x04000298)
@;	str r5, [r4, #0x1c]		@; guardar cero en parte alta DIV_NUMER (0x0400029C)
@;.Ldiv_wait:
@;	ldrh r5, [r4]			@; bucle para esperar a que el hardware de división
@;	tst r5, #0x8000			@; termine el cálculo (¡espera ACTIVA!)
@;	bne .Ldiv_wait
@;	tst r5, #0x4000			@; test del bit de error
@;	beq .Ldiv_noerr
@;	mov r0, r5				@; código de error
@;	b .Ldiv_fin
@;.Ldiv_noerr:
@;	ldr r5, [r4, #0x20]		@; R5 = cociente, desde DIV_RESULT (0x040002A0)
@;	str r5, [r2]			@; guardar resultado en dirección (*quo);
@;	ldr r5, [r4, #0x28]		@; R5 = resto, desde DIVREM_RESULT (0x040002A8)
@;	str r5, [r2]			@; guardar resultado en dirección (*mod);
@;	mov r0, #0				@; código de retorno OK
@;.Ldiv_fin:
@;	pop {r4, r5, pc}
	@;¡FIN del código comentado!
	@;ATENCIÓN: en caso de descomentar el código anterior, la división pasaría
	@;a realizarse CON signo.
	push {r4-r7, lr}
	cmp r1, #0				@; verificar si se está intentando dividir por cero
	bne .Ldiv_ini
	mov r0, #1				@; código de error
	b .Ldiv_fin2
.Ldiv_ini:
	mov r4, #0				@; R4 es el cociente (q)
	mov r5, #0				@; R5 es el resto (r)
	mov r6, #31				@; R6 es índice del bucle (de 31 a 0)
	mov r7, #0xff000000
.Ldiv_for1:
	tst r0, r7				@; comprobar si hay bits activos en una zona de 8
	bne .Ldiv_for2			@; bits del numerador, para evitar rastreo 32 bits
	mov r7, r7, lsr #8
	sub r6, #8				@; 8 bits menos a buscar
	cmp r7, #0
	bne .Ldiv_for1
	b .Ldiv_fin1			@; caso especial (numerador = 0 -> q=0 y r=0)
.Ldiv_for2:
	mov r7, r0, lsr r6		@; R7 es variable de trabajo j;
	and r7, #1				@; j = bit i-ésimo del numerador; 
	mov r5, r5, lsl #1		@; r = r << 1;
	orr r5, r7				@; r = r | j;
	mov r4, r4, lsl #1		@; q = q << 1;
	cmp r5, r1
	blo .Ldiv_cont			@; si (r >= divisor), activar bit en cociente
	sub r5, r1				@; r = r - divisor;
	orr r4, #1				@; q = q | 1;
 .Ldiv_cont:
	sub r6, #1				@; decrementar índice del bucle
	cmp r6, #0
	bge .Ldiv_for2			@; bucle for-2, mientras i >= 0
.Ldiv_fin1:
	str r4, [r2]
	str r5, [r3]			@; guardar resultados en memoria (por referencia)
	mov r0, #0				@; código de OK
.Ldiv_fin2:
	pop {r4-r7, pc}


	.global _gi_num2str
	@;Parámetros
	@; R0: char * numstr,
	@; R1: unsigned int length,
	@; R2: unsigned int num
	@;Resultado
	@; R0: 0 si no hay problema, !=0 si el número no cabe en el string
_gi_num2str:
	push {r3-r8, lr}
	cmp r1, #1
	bhi .Ln2s_cont1			@; verificar si hay espacio para centinela + 1 dígito
	mov r0, #-1
	b .Ln2s_fin				@; retornar con código de error en R0
.Ln2s_cont1:
	mov r6, r0				@; R6 = puntero a string de resultado
	mov r7, r1				@; R7 = índice carácter (inicialmente es la longitud del string)
	mov r8, r2				@; R8 = número a transcribir
	mov r3, #0
	sub r7, #1
	strb r3, [r6, r7]		@; guardar final de string (0) en última posición
.Ln2s_while:	
	cmp r7, #0				@; repetir mientras quede espacio en el string
	beq .Ln2s_cont2
	sub sp, #8				@; reservar espacio en la pila para resultados
	mov r0, r8				@; pasar numerador por valor
	mov r1, #10				@; pasar denominador por valor
	mov r2, sp				@; pasar dirección para albergar el cociente
	add r3, sp, #4			@; pasar dirección para albergar el resto
	bl _gi_divmod
	pop {r4-r5}				@; R4 = cociente, R5 = resto
	add r5, #48				@; añadir base de códigos ASCII para dígitos numéricos
	sub r7, #1
	strb r5, [r6, r7]		@; almacenar código ASCII en vector
	mov r8, r4				@; actualizar valor del número a convertir
	cmp r8, #0				@; repetir mientras el número sea diferente de 0
	bne .Ln2s_while
.Ln2s_pad:
	cmp r7, #0				@; bucle para llenar de espacios en blanco la
	beq .Ln2s_cont2			@; parte restante del inicio del string
	mov r3, #' '
	sub r7, #1
	strb r3, [r6, r7]		@; almacenar código de espacio en blanco en vector
	b .Ln2s_pad
.Ln2s_cont2:
	mov r0, r8				@; esto indica si el numero se ha podido codificar
.Ln2s_fin:					@; completamente en el string (si R0 = 0)
	pop {r3-r8, pc}


	.global _gi_print
	@;Parámetros
	@; R0: char * message
_gi_print:
	push {r1-r2, lr}
	ldr r2, =_gd_pidz		@; R2 = dirección _gd_pidz
	ldr r1, [r2]
	and r1, #0xf			@; R1 = zócalo actual
	bl _gg_escribir
	pop {r1-r2, pc}


.end

