@;==============================================================================
@;
@;	"garlic_itcm_proc.s":	código de las funciones de control de procesos.
@;						(ver "garlic_system.h" para descripción de funciones)
@;
@;==============================================================================

.section .itcm,"ax",%progbits

	.arm
	.align 2
	
	.global _gp_WaitForVBlank
	@; rutina para pausar el procesador mientras no se produzca una interrupción
	@; de retrazado vertical (IRQ_VBL); es un sustituto de la "swi #5", que
	@; evita la necesidad de cambiar a modo supervisor en los procesos GARLIC
_gp_WaitForVBlank:
	push {r0-r1, lr}
	ldr r0, =__irq_flags
.Lwait_espera:
	mcr p15, 0, lr, c7, c0, 4	@; HALT (suspender hasta nueva interrupción)
	nop						@; instrucción de relleno, para asegurar que se
	ldr r1, [r0]			@; ejecuta la "ldr r1, [r0]" al salir de HALT
	tst r1, #1				@; comprobar flag IRQ_VBL
	beq .Lwait_espera		@; repetir bucle mientras no exista IRQ_VBL
	bic r1, #1
	str r1, [r0]			@; poner a cero el flag IRQ_VBL
	pop {r0-r1, pc}


	.global _gp_IntrMain
	@; Manejador principal de interrupciones del sistema Garlic
_gp_IntrMain:
	mov	r12, #0x4000000
	add	r12, r12, #0x208	@; R12 = base registros de control de interrupciones	
	ldr	r2, [r12, #0x08]	@; R2 = REG_IE (máscara de bits con int. permitidas)
	ldr	r1, [r12, #0x0C]	@; R1 = REG_IF (máscara de bits con int. activas)
	and r1, r1, r2			@; filtrar int. activas con int. permitidas
	ldr	r2, =irqTable
.Lintr_find:				@; buscar manejadores de interrupciones específicos
	ldr r0, [r2, #4]		@; R0 = máscara de int. del manejador indexado
	cmp	r0, #0				@; si máscara = cero, fin de vector de manejadores
	beq	.Lintr_setflags		@; (abandonar bucle de búsqueda de manejador)
	ands r0, r0, r1			@; determinar si el manejador indexado atiende a una
	beq	.Lintr_cont1		@; de las interrupciones activas
	ldr	r3, [r2]			@; R3 = dirección de salto del manejador indexado
	cmp	r3, #0
	beq	.Lintr_ret			@; abandonar si dirección = 0
	mov r2, lr				@; guardar dirección de retorno
	blx	r3					@; invocar el manejador indexado
	mov lr, r2				@; recuperar dirección de retorno
	b .Lintr_ret			@; salir del bucle de búsqueda
.Lintr_cont1:	
	add	r2, r2, #8			@; pasar al siguiente índice del vector de
	b	.Lintr_find			@; manejadores de interrupciones específicas
.Lintr_ret:
	mov r1, r0				@; indica qué interrupción se ha servido
.Lintr_setflags:
	str	r1, [r12, #0x0C]	@; REG_IF = R1 (comunica interrupción servida)
	ldr	r0, =__irq_flags	@; R0 = dirección flags IRQ para gestión IntrWait
	ldr	r3, [r0]
	orr	r3, r3, r1			@; activar el flag correspondiente a la interrupción
	str	r3, [r0]			@; servida (todas si no se ha encontrado el maneja-
							@; dor correspondiente)
	mov	pc,lr				@; retornar al gestor de la excepción IRQ de la BIOS


	.global _gp_rsiVBL
	@; Manejador de interrupciones VBL (Vertical BLank) de Garlic:
	@; se encarga de actualizar los tics, intercambiar procesos, etc.
_gp_rsiVBL:
	push {r4-r7, lr}
	ldr r4, =_gd_tickCount		@; cargamos la direccion de la variable tickCount
	ldr r5, [r4]				@; cargamos en r5 el valor de la variable tickCount
	add r5, #1					@; incrementamos el contador de tics
	str r5, [r4]				@; guardamos el valor en la variable
	ldr r4, =_gd_nReady			@; cargamos la direccion de la variable numReady
	ldr r5, [r4]				@; cargamos en r5 el valor de la variable numReady
	ldr r4, =_gd_pidz			@; cargamos la direccion de la variable pidz
	ldr r7, [r4]				@; cargamos en r7 el valor de la variable pidz
	cmp r5, #0					@; comprobamos si existe algun proceso en la cola de Ready
	beq .LrsiVBL_End			@; en caso de que numReady sea 0 saltamos al final de la RSI
	cmp r7, #0					@; comprobamos si se trata del proceso de sistema operativo
	beq .LrsiVBL_Save			@; en caso de que sea correcto saltamos a salvar proceso
	mov r6, #0xffffff0			@; cargamos en r6 el valor de los 28 bits altos en 1
	and r6, r7					@; hacemos un and para saber si pid=0
	cmp r6, #0					
	bne .LrsiVBL_Save			@; en caso de que no sea igual a 0 hay que salvar el contexto
	mov r6, #0x0000000f			@; cargamos en r6 el valor de los 4 bits bajos en 1
	and r6,r7					@; hacemos un and para saber si z=0
	cmp r6,#0
	bne .LrsiVBL_End			@; en caso de que no sea igual a 0 el proceso ha terminado su ejecuccion
.LrsiVBL_Save:
	ldr r4, =_gd_nReady			@; cargamos la direccion de la variable numReady
	ldr r5, [r4]				@; cargamos en r5 el valor de la variable numReady
	ldr r6, =_gd_pidz			@; cargamos la direccion de la variable pidz
	ldr r7, [r4]				@; cargamos en r7 el valor de la variable pidz
	bl _gp_salvarProc			@; llamamos a la funcion de salvar
	bl _gp_restaurarProc		@; llamamos a la funcion de restaurar	
.LrsiVBL_End:
	pop {r4-r7, pc}


	@; Rutina para salvar el estado del proceso interrumpido en la entrada
	@; del vector _gd_psv correspondiente al zócalo actual
	@;Parámetros
	@; R4: dirección _gd_nReady
	@; R5: número de procesos en READY
	@; R6: dirección _gd_pidz
	@; R7: PIDz
	@;Resultado
	@; R5: nuevo número de procesos en READY (+1)
_gp_salvarProc:
	push {r8-r11, lr}
	mov r9, #0x0000000f			@; cargamos en r9 el valor de los 4 bits bajos en 1
	and r9,r7					@; hacemos un and para tener solo el valor de z
	ldr r8, =_gd_qReady			@; cargamos en r8 la direccion del vector
	strb r9, [r8, r5]			@; guardamos el zocalo en la ultima posicion
	add r5, #1					@; incrementamos la variable numReady
	str r5, [r4]				@; guardamos el valor en la variable
	ldr r10, [sp, #0x3c]		@; guardamos el PC del proceso actual en r10
	ldr r11,=_gd_psv			@; cargamos en r11 la direccion del psv
	mov r8, #0x40				
	mul  r9, r8 		
	add r8, r9, #0x4			@; la direccion en la que tenemos que guardar es el (zocalox64) + 4 
	str r10, [r11, r8] 			@; guardar el PC de la IRQ en el campo PC de _gd_psv[z]
	mrs r10, CPSR				@; instruccion para copiar el cPRS en un registro
	mov r8, #0x40
	mul r9, r8
	add r8, r9, #0xc			
	str r10, [r11, r8]			@; guardar el CPSR en en el campo Status de _gd_psv[z]
	mov r8, sp					@; guardamos el SP_irq en el registro r8
	mrs r10, SPSR				@; SPSR contiene el estado anterior del proceso
	cmp r10, #0x10				@; comparamos para saber si el estado anterior era de usuario
	bne .LSysMod				@; si es diferente al codigo User pasamos
	mov r10, #0x1f				@; en caso de ser igual lo cambiamos a modo system
.LSysMod:
	msr CPSR, r10				@; cambiamos a modo de ejecucion 
	ldr r9, [r8, #0x28]			@; cargamos el registro r0 de la IRQ para guardarlos en la pila de procesos
	push {r9}
	ldr r9, [r8, #0x2c]			@; cargamos el registro r1
	push {r9}
	ldr r9, [r8, #0x30]			@; cargamos el registro r2
	push {r9}
	ldr r9, [r8, #0x34]			@; cargamos el registro r3
	push {r9}
	ldr r9, [r8, #0x14]			@; cargamos el registro r4
	push {r9}
	ldr r9, [r8, #0x18]			@; cargamos el registro r5
	push {r9}
	ldr r9, [r8, #0x1c]			@; cargamos el registro r6
	push {r9}
	ldr r9, [r8, #0x20]			@; cargamos el registro r7
	push {r9}
	ldr r9, [r8]				@; cargamos el registro r8
	push {r9}
	ldr r9, [r8, #0x4]			@; cargamos el registro r9
	push {r9}
	ldr r9, [r8, #0x8]			@; cargamos el registro r10
	push {r9}
	ldr r9, [r8, #0xc]			@; cargamos el registro r11
	push {r9}
	ldr r9, [r8, #0x38]			@; cargamos el registro r12
	push {r9}
	ldr r9, [r8, #0x10]			@; cargamos el registro r14
	push {r9}
	mov r8, #0x40				
	mov r9, #0x0000000f			@; cargamos en r9 el valor de los 4 bits bajos en 1
	and r9,r7					@; hacemos un and para tener solo el valor de z
	mul  r9, r8 				
	add r8, r9, #0x8			@; la direccion en la que tenemos que guardar es el (zocalox64) + 8
	str sp, [r11, r8]			@; guardamos el sp del proceso en el campo sp del psv
	mov r8, #0x12				@; guardamos en r8 el modo IRQ
	msr CPSR, r8				@; cambiamos a modo irq
	
	pop {r8-r11, pc}


	@; Rutina para restaurar el estado del siguiente proceso en la cola de READY
	@;Parámetros
	@; R4: dirección _gd_nReady
	@; R5: número de procesos en READY
	@; R6: dirección _gd_pidz
_gp_restaurarProc:
	push {r8-r11, lr}
	
	pop {r8-r11, pc}


	.global _gp_numProc
	@;Resultado
	@; R0: número total de procesos cargados en el sistema
_gp_numProc:
	push {r1-r2, lr}
	mov r0, #1				@; contar siempre 1 proceso en RUN
	ldr r1, =_gd_nReady
	ldr r2, [r1]			@; R2 = número de procesos en cola de READY
	add r0, r2				@; añadir procesos en READY
	pop {r1-r2, pc}


	.global _gp_crearProc
	@; prepara un proceso para ser ejecutado, creando su entorno de ejecución y
	@; colocándolo en la cola de READY
	@;Parámetros
	@; R0: intFunc funcion,
	@; R1: int zocalo,
	@; R2: char *nombre
	@;Resultado
	@; R0: 0 si no hay problema, >0 si no se puede crear el proceso
_gp_crearProc:
	push {r1-r8, lr}
	
	pop {r1-r8, pc}


	@; Función para terminar un proceso de usuario:
	@; borra el PID del PSB del zócalo actual, para indicar que esa entrada
	@; del vector _gd_psv está libre; también elimina el PID de la variable
	@; _gd_pidz (sin modificar el número de zócalo), para que el procedimiento
	@; de intercambio de procesos NO salve el estado del proceso terminado.
_gp_terminarProc:
	ldr r0, =_gd_pidz
	ldr r1, [r0]			@; R1 = valor actual de PID + zócalo
	and r1, r1, #0xf		@; R1 = zócalo del proceso desbancado
	str r1, [r0]			@; guardar zócalo con PID = 0, para no salvar estado			
	ldr r2, =_gd_psv
	add r2, r1, lsl #6		@; R2 = dirección base _gd_psv[zocalo]
	mov r3, #0
	str r3, [r2]			@; borrar PID del bloque de estado del proceso
	bl _gp_WaitForVBlank	@; pausar procesador

	
.end

