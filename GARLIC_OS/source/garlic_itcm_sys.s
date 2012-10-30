@;==============================================================================
@;
@;	"garlic_itcm_sys.s":	c�digo de las funciones de soporte al sistema.
@;						(ver "garlic_system.h" para descripci�n de funciones)
@;
@;==============================================================================

.section .itcm,"ax",%progbits

	.arm
	.align 2
	
	
		.global _gs_borrarVentana
	@; Rutina para borrar todo el contenido de la ventana de un z�calo, vaciar
	@;	el buffer de caracteres y poner el puntero de l�nea actual al principio
	@;	de la ventana (fila 0)
	@;Par�metros
	@;	R0: �ndice de ventana (0..15)
	@;	R1: n�mero de ventanas (0 -> 4 ventanas, 1 -> 16 ventanas)
	@;�ATENCI�N! Para que esta rutina funcione correctamente, es necesario
	@; haber inicializado el procesador gr�fico principal correctamente
	@; (baldosas, paleta, mapa de baldosas de fondo 2 con las dimensiones
	@;  adecuadas, etc.).
_gs_borrarVentana:
	push {r0-r5, lr}
								@; (para procesador gr�fico principal)
	mov r4, #0x06000000			@; R4 = base 0 del mapa de baldosas (fondo 2)
	and r0, #0x0F
	and r1, #1
	mov r2, #24
	mov r2, r2, lsl #7
	mov r2, r2, lsl r1			@; R2 = 24 * 2 * (64 o 128)
	mov r3, r0, lsr #1
	mov r3, r3, lsr r1			@; R3 = iv / (2 o 4)
	mul r5, r3, r2
	add r4, r5					@; R4 = base primera fila de ventana
								@;		4 ventanas : +(24 * 2 * 64 * iv / 2)
								@;		16 ventanas: +(24 * 2 * 128 * iv / 4)
	mov r2, #1
	mov r2, r2, lsl r1			@; R2 = 1 para 4 ventanas
	add r2, r1					@; R2 = 3 para 16 ventanas
	and r2, r0					@; R2 = (iv % 2) o (iv % 4)
	mov r2, r2, lsl #6
	add r4, r2					@; R4 = base primera casilla de ventana
								@;		4 ventanas : (iv % 2 * 32*2)
								@;		16 ventanas: (iv % 4 * 32*2)
	mov r2, #128
	mov r1, r2, lsl r1			@; R1 = 64 o 128 caracteres por fila
	mov r5, #0
	mov r2, #0
.LborrarV_for1:					@; bucle para borrar filas
	mov r3, #0
.LborrarV_for2:					@; bucle para recorrer las casillas de la fila
	strh r5, [r4, r3]			@; borrar casilla
	add r3, #2					@; pasar a siguiente casilla
	cmp r3, #64					@; 32 columnas
	blo .LborrarV_for2
	add r4, r1					@; pasar a siguiente fila
	add r2, #1
	cmp r2, #24					@; 24 filas
	blo .LborrarV_for1
	
	ldr r4, =_gd_psv
	mov r1, #92
	mul r2, r0, r1
	add r4, r2					@; R4 = base _gd_psv[z] (z = iv)
	str r5, [r4, #24]			@; _gd_psv[z].pControl = 0
	
	pop {r0-r5, pc}



	.global _gs_pintarFranjas
	@; Rutina para para pintar las franjas verticales correspondientes a los
	@; trozos de memoria indicados por par�metro, con el color tambi�n indicado
	@; por par�metro.
	@;Par�metros
	@;	R0: la base de las baldosas correspondientes a las franjas
	@;	R1: el �ndice inicial de las franjas
	@;	R2: el n�mero de franjas a pintar
	@;	R3: el color que hay que utilizar
	@;�ATENCI�N! Para que esta rutina funcione correctamente, es necesario
	@; haber inicializado el procesador gr�fico secundario con fondo 0 de texto
	@; (32x32) caracteres, memoria de v�deo en el banco de v�deo-RAM C, mapa
	@; en la base 0, 512 baldosas (8 bits por p�xel) en la base 1, y crear
	@; las 96 baldosas correspondientes a los gr�ficos de memoria, adem�s de
	@; haver copiado dichas baldosas en sus respectivas posiciones de pantalla.
_gs_pintarFranjas:
	push {r0-r7, lr}
								@; (para procesador gr�fico secundario)
	and r4, r1, #7				@; R4 = �nidice franja inicial m�dulo 8
	mov r1, r1, lsr #3			@; R1 = �ndice inicial dividido por 8
	add r1, #512				@; saltar a�adir 512 baldosas (caracteres)
	mov r1, r1, lsl #6			@; multiplicar todo por 64 bytes por baldosa
	add r0, r1					@; R0 apunta a la primera baldosa de memoria
	mov r1, r3, lsl #8			@; R1 = color << 8
.LpintarFranjas_while:
	cmp r2, #0					@; comprobar si final de bucle de franjas
	beq .LpintarFranjas_fiwhile
	mov r5, #0
.LpintarFranjas_for:			@; bucle para 4 p�xeles verticales por franja
	add r6, r4, r5
	add r6, #16
	ldrh r7, [r0, r6]			@; R7 = valor 2 bytes de baldosa a modificar
	tst r4, #1
	beq .LpintarFranjas_bbajo
	and r7, #0xFF				@; limpiar byte alto
	orr r7, r1					@; fijar byte alto
	b .LpintarFranjas_cont
.LpintarFranjas_bbajo:
	bic r7, #0xFF				@; limpiar byte bajo
	orr r7, r3					@; fijar byte bajo
.LpintarFranjas_cont:
	strh r7, [r0, r6]			@; actualizar memoria de v�deo
	add r5, #8
	cmp r5, #32
	bne .LpintarFranjas_for		@; cerrar bucle 4 p�xeles
	add r4, #1					@; avanzar puntero de columna
	cmp r4, #8
	bne .LpintarFranjas_cont2	@; verificar si se llega a la �ltima columna
	mov r4, #0
	add r0, #64					@; avanzar puntero de baldosa
.LpintarFranjas_cont2:
	sub r2, #1					@; decrementar n�mero de franjas a pintar
	b .LpintarFranjas_while
.LpintarFranjas_fiwhile:

	pop {r0-r7, pc}



	.global _gs_escribirStringSub
	@; Rutina para escribir un (terminado con centinela cero) a partir de la
	@; posici�n indicada por par�metros (fil, col), con el color especificado,
	@; en la pantalla secundaria;
	@;Par�metros
	@;	R0: direcci�n inicial del string
	@;	R1: fila inicial
	@;	R2: columna inicial
	@;	R3: color del texto
	@;�ATENCI�N! Para que esta rutina funcione correctamente, es necesario
	@; haber inicializado el procesador gr�fico secundario con fondo 0 de texto
	@; (32x32) caracteres, memoria de v�deo en el banco de v�deo-RAM C, mapa
	@; en la base 0, 128 baldosas (8 bits por p�xel) en la base 1, y dibujos de
	@; baldosas repetidos 4 veces, una por cada color.
_gs_escribirStringSub:
	push {r0-r4, lr}
								@; (para procesador gr�fico secundario)
	mov r4, #0x06200000			@; R4 = base 0 del mapa de baldosas 
	and r1, #0x1F
	cmp r1, #24					@; R1 = fila inicial (filtrar 5 bits bajos)
	blo .LescStrSub_0
	sub r1, #24					@; evita valores > 24
.LescStrSub_0:
	and r2, #0x1F				@; R2 = columna inicial (evita valores > 31)
	and r3, #0x03				@; R3 = color (s�lo admite valores 0..3)
	mov r3, r3, lsl #7			@; R3 = desplazamiento baldosas de color (* 128)
	add r4, r1, lsl #6
	add r4, r2, lsl #1			@; R4 = base mapa + (fila * 32 + columna) * 2 
.LescStrSub_while:
	ldrb r1, [r0]				@; R1 = string[i]
	cmp r1, #32					@; salir de bucle si detecta centinela (R1 = 0),
	blo .LescStrSub_fiwhile	 	@; o c�digo ASCII no imprimible (R1 < 32)
	sub r1, #32					@; restar base c�digos ASCII imprimibles
	add r1, r3					@; sumar base de baldosas de color
	strh r1, [r4]				@; fijar baldosa
	add r4, #2					@; R4 apunta a la siguiente posicion del mapa
	add r0, #1					@; avanzar puntero de string
	add r2, #1					@; avanzar �ndice de columna
	cmp r2, #32
	blo .LescStrSub_while		@; seguir mientras no llegue a �ltima columna
.LescStrSub_fiwhile:
	pop {r0-r4, pc}



	.global _gs_rsiTIMER0
	@; Rutina de Servicio de Interrupci�n (RSI) para contabilizar los tics
	@; de trabajo de cada proceso: suma los tics de todos los procesos y calcula
	@; el porcentaje de uso de la CPU, que se guarda en los 8 bits altos de la
	@; entrada _gd_psv[z].workTicks de cada proceso (z) y, si el procesador
	@; gr�fico secundario est� correctamente configurado, se imprime en la
	@; columna correspondiente de la tabla de procesos.
_gs_rsiTIMER0:
	push {r0-r8, lr}

	ldr r4, =_gd_sincMain
	ldr r1, [r4]
	orr r1, #1				@; activar bit de sincronismo de temporizador
	str r1, [r4]
	ldr r4, =_gd_psv		@; R4 = puntero a entrada _gd_psv[0]
	mov r8, #0				@; R8 = total de tics de trabajo
	mov r6, #0				@; R6 = contador de z�calos (de 0 a 15)
	mov r7, #100			@; R7 = 100 (constante)

	@; primer bucle para contabilizar n�mero de tics total
						@; (y borrar el porcentaje de uso)
.LrsiTIMER0_for1:
	ldr r5, [r4, #20]
	mov r5, r5, lsl #8
	mov r5, r5, lsr #8		@; R5 = tics de trabajo (sin porcentaje de uso)
	str r5, [r4, #20]
	add r8, r5
.LrsiTIMER0_cont1:
	add r6, #1				@; incrementar contador de z�calos
	cmp r6, #16
	beq .LrsiTIMER0_fifor1
	add r4, #92				@; avanzar puntero _gd_psv[z]
	ldr r5, [r4]			@; R5 = PID del proceso
	cmp r5, #0
	beq .LrsiTIMER0_cont1	@; si PID = 0, no contabilizar tics
	b .LrsiTIMER0_for1		@; cerrar bucle
.LrsiTIMER0_fifor1:
	
	sub sp, #8				@; crear espacio en pila IRQ
	ldr r4, =_gd_psv		@; R4 = puntero a entrada _gd_psv[0]
	mov r6, #0

	@; segundo bucle para guardar (y escribir) el porcentaje de uso
						@; (y borrar los tics de trabajo)
.LrsiTIMER0_for2:
	ldr r5, [r4, #20]
	mul r0, r5, r7
	mov r1, r8
	mov r2, sp				@; R2, R3 = punteros a zona de memoria con resultado
	add r3, sp, #4				@; de la divisi�n
	bl _gi_divmod
	ldr r5, [r2]			@; R5 = tics * 100 / total (porcentaje de uso)
	mov r5, r5, lsl #24
	str r5, [r4, #20]		@; guarda %uso en _gd_psv[z].workTicks(8 bits altos)
	mov r0, sp				@; R0 = direccion string (pila IRQ)
	mov r1, #4
	mov r2, r5, lsr #24		@; n�mero a convertir
	bl _gi_num2str
	mov r0, sp
	add r1, r6, #4			@; escribir en linea de tabla (zocalo+4)
	mov r2, #28
	mov r3, #0
	bl _gs_escribirStringSub
.LrsiTIMER0_cont2:
	add r6, #1				@; incrementar contador de z�calos
	cmp r6, #16
	beq .LrsiTIMER0_fifor2
	add r4, #92				@; avanzar puntero _gd_psv[z]
	ldr r5, [r4]			@; R5 = PID del proceso
	cmp r5, #0
	beq .LrsiTIMER0_cont2	@; si PID = 0, no contabilizar tics
	b .LrsiTIMER0_for2		@; cerrar bucle
.LrsiTIMER0_fifor2:
	add sp, #8				@; elimiar espacio de pila para resultados divisi�n
	
	pop {r0-r8, pc}



	.global _gs_rsiTIMER1
	@; Rutina de Servicio de Interrupci�n (RSI) para actualizar la representa-
	@; ci�n de la pila y el estado de la CPU de los procesos activos.
_gs_rsiTIMER1:
	push {r0-r7, lr}

	ldr r4, =_gd_psv		@; R4 = puntero a entrada _gd_psv[0]
	ldr r5, [r4, #8]		@; R5 = SP actual del proceso de Sistema Operativo
	mov r6, #0				@; R6 = contador de z�calos (de 0 a 15)
	ldr r7, =_gd_stacks		@; R7 = base de las pilas de procesos de programa
	mov r0, #0xB000000
	add r0, #0x3D00			@; R0 = base de la pila del S.O.
	cmp r5, #0
	bne .LrsiTIMER1_SPn0	@; si (SP != 0), continuar normalmente
	mov r2, #0				@; sino, caso especial (inicio del S.O.)
	b .LrsiTIMER1_cont0
.LrsiTIMER1_SPn0:
	sub r0, r5
	mov r0, r0, lsr #2		@; R0 = posiciones (words) ocupadas en pila del S.O.
	@;mov r1, #1600			@; R1 = posiciones disponibles
	@;mov r0, r0, lsl #4	@; R0 = posiciones * 16
	@;sub sp, #8			@; crear espacio en pila IRQ
	@;mov r2, sp			@; R2, R3 = punteros a zona de memoria con resultado
	@;add r3, sp, #4		@; de la divisi�n
	@;bl _gi_divmod			@; posiciones * 16 / 1600 = posiciones / 100
	@;pop {r2-r3}			@; R2 = cociente (R3 elimina espacio pila IRQ)	
	mov r2, r0, lsr #4		@; R2 = posiciones ocupadas / 16 	(1/6 del total)
	b .LrsiTIMER1_cont0		@; continua los c�lculos como un proceso normal

	@; bucle para c�lculo y visualizaci�n de la ocupaci�n de la pila
.LrsiTIMER1_for1:
	ldr r5, [r4]			@; R5 = PID del proceso del z�calo actual
	cmp r5, #0
	beq .LrsiTIMER1_cont3	@; evitar c�lculos si PID = 0 (excepto para S.O.)
	ldr r5, [r4, #8]		@; R5 = SP actual del proceso
	mov r3, r6, lsl #9		@; R3 = n�mero de z�calo * 128 words (2^9 bytes)
	add r3, r7
	sub r3, #4				@; base de la pila del proceso
	sub r3, r5
	mov r3, r3, lsr #2		@; R3 = posiciones (words) ocupadas en pila normal
	mov r2, r3, lsr #3		@; R2 = n�mero de marcas de pila (hasta 16)
.LrsiTIMER1_cont0:
	sub sp, #4				@; crea espacio para 'string' en pila IRQ
	mov r1, #(119+32)		@; R1 = marca de pila vac�a
	cmp r2, #8
	bhs .LrsiTIMER1_cont1
	add r2, r1				@; R2 = primera marca de pila
	orr r2, r1, lsl #8		@; a�adir segunda marca de pila vac�a
	b .LrsiTIMER1_cont2
.LrsiTIMER1_cont1:
	sub r2, #8				@; restar 8 unidades de la primera marca
	add r2, r1				@; R2 = segunda marca de pila
	mov r2, r2, lsl #8
	mov r1, #(127+32)
	orr r2, r1				@; a�adir primera marca de pila llena
.LrsiTIMER1_cont2:
	str r2, [sp]			@; guardar marcas en 'string' (con centinela)
	mov r0, sp
	add r1, r6, #4			@; escribir en linea de tabla (zocalo+4)
	mov r2, #23
	mov r3, #0
	bl _gs_escribirStringSub
	add sp, #4				@; restaurar puntero de pila IRQ
.LrsiTIMER1_cont3:
	add r4, #92				@; avanzar puntero _gd_psv[z]
	add r6, #1				@; incrementar contador de z�calos
	cmp r6, #16
	bne .LrsiTIMER1_for1

	@; visualizaci�n del proceso en RUN
	ldr r7, =_gd_pidz
	ldr r7, [r7]
	and r7, #0x0f			@; R7 = z�calo del proceso en ejecuci�n
	mov r2, #'R'			@; marca 'R': estado RUN
	sub sp, #4
	str r2, [sp]
	mov r0, sp
	add r1, r7, #4			@; escribir en linea de tabla (zocalo+4)
	mov r2, #26
	mov r3, #1
	bl _gs_escribirStringSub
	add sp, #4				@; restaurar puntero de pila IRQ

	@; visualizaci�n de los procesos en READY
	ldr r5, =_gd_nReady
	ldr r5, [r5]			@; R5 = n�mero de procesos en READY
	cmp r5, #0
	beq .LrsiTIMER1_cont4
	ldr r7, =_gd_qReady
	mov r6, #'Y'			@; marca 'Y': estado READY
	sub sp, #4				@; crea espacio para 'string' en pila IRQ
	str r6, [sp]
	mov r0, sp
	mov r2, #26
	mov r3, #0
.LrsiTIMER1_for2:
	ldrb r4, [r7]			@; R4 = n�mero de z�calo en READY
	add r1, r4, #4
	bl _gs_escribirStringSub
	add r7, #1
	subs r5, #1
	bne .LrsiTIMER1_for2
	add sp, #4				@; restaurar puntero de pila IRQ
.LrsiTIMER1_cont4:

	@; visualizaci�n de los procesos en DELAY (bloqueados)
	ldr r5, =_gd_nDelay
	ldr r5, [r5]			@; R5 = n�mero de procesos en DELAY
	cmp r5, #0
	beq .LrsiTIMER1_fin
	ldr r7, =_gd_qDelay
	mov r6, #'B'			@; marca 'B': estado BLOQUED
	sub sp, #4				@; crea espacio para 'string' en pila IRQ
	str r6, [sp]
	mov r0, sp
	mov r2, #26
	mov r3, #0
.LrsiTIMER1_for3:
	ldr r4, [r7]			@; R4 = n�mero de z�calo + tics pendientes en DELAY
	mov r4, r4, lsr #24		@; filtrar n�mero de z�calo
	add r1, r4, #4
	bl _gs_escribirStringSub
	add r7, #4				@; avanzar 4 bytes (la cola DELAY es de words)
	subs r5, #1
	bne .LrsiTIMER1_for3
	add sp, #4				@; restaurar puntero de pila IRQ

.LrsiTIMER1_fin:
	pop {r0-r7, pc}


.end

