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
	
	@; incrementamos la variable tickCount 
	
	ldr r4, =_gd_tickCount		@; cargamos la direccion de la variable tickCount
	ldr r5, [r4]				@; cargamos en r5 el valor de la variable tickCount
	add r5, #1					@; incrementamos el contador de tics
	str r5, [r4]				@; guardamos el valor en la variable
	
	@; comprobamos el numero de procesos en la cola de Ready
	
	ldr r4, =_gd_nReady			@; cargamos la direccion de la variable numReady
	ldr r5, [r4]				@; cargamos en r5 el valor de la variable numReady
	cmp r5, #0					@; comprobamos si existe algun proceso en la cola de Ready
	beq .LrsiVBL_End			@; en caso de que numReady sea 0 saltamos al final de la RSI
	
	@; en caso de que haya comprobamos si es el proceso de sistema o no
	
	ldr r4, =_gd_pidz			@; cargamos la direccion de la variable pidz
	ldr r7, [r4]				@; cargamos en r7 el valor de la variable pidz
	cmp r7, #0					@; comprobamos si se trata del proceso de sistema operativo
	beq .LrsiVBL_Save			@; en caso de que sea correcto saltamos a salvar proceso
	
	@; en caso de que no comprobamos si el proceso ha terminado su ejecucion
	
	mov r6, #0xfffffff0			@; cargamos en r6 el valor de los 28 bits altos en 1
	and r6, r7					@; hacemos un and para saber si pid=0
	cmp r6, #0					
	bne .LrsiVBL_Save			@; en caso de que no sea igual a 0 hay que salvar el contexto
	mov r6, #0xf				@; cargamos en r6 el valor de los 4 bits bajos en 1
	and r6,r7					@; hacemos un and para saber si z=0
	cmp r6,#0
	bne .LrsiVBL_Rest			@; en caso de que no sea igual a 0 el proceso ha terminado su ejecuccion

	@; pasamos a salvar el proceso
	
.LrsiVBL_Save:
	ldr r4, =_gd_nReady			@; cargamos la direccion de la variable numReady
	ldr r5, [r4]				@; cargamos en r5 el valor de la variable numReady
	ldr r6, =_gd_pidz			@; cargamos la direccion de la variable pidz
	ldr r7, [r6]				@; cargamos en r7 el valor de la variable pidz
	bl _gp_salvarProc			@; llamamos a la funcion de salvar
	
	@; pasamos a restaurar el proceso
	
.LrsiVBL_Rest:
	ldr r4, =_gd_nReady			@; cargamos la direccion de la variable numReady
	ldr r5, [r4]				@; cargamos en r5 el valor de la variable numReady
	ldr r6, =_gd_pidz			@; cargamos la direccion de la variable pidz
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
	
	@; primero guardamos el zocalo del proceso a desbancar en la cola de Ready
	
	mov r9, #0xf				@; cargamos en r9 el valor de los 4 bits bajos en 1
	and r9,r7					@; hacemos un and para tener solo el valor de z
	ldr r8, =_gd_qReady			@; cargamos en r8 la direccion del vector
	strb r9, [r8, r5]			@; guardamos el zocalo en la ultima posicion
	
	@; incrementamos el contador de procesos pendientes 
	
	add r5, #1					@; incrementamos la variable numReady
	str r5, [r4]				@; guardamos el valor en la variable
	
	@; guardamos el pc del proceso a desbancar en el campo PC del vector psv de ese proceso
	
	ldr r10, [sp, #0x3c]		@; guardamos el PC del proceso actual en r10
	ldr r11,=_gd_psv			@; cargamos en r11 la direccion del psv

	lsl r9, #6					@; desplazamos 6 posiciones el zocalo que es lo mismo que multiplicarlo por 64
	add r8, r9, #0x4			@; la direccion en la que tenemos que guardar es el (zocalox64) + 4 

	str r10, [r11, r8] 			@; guardar el PC de la pila de la IRQ en el campo PC de _gd_psv[z]
	
	@; guardamos el SPSR del proceso a desbancar porque buscamos el modo anterior en el campo Status
	
	mrs r10, SPSR				@; instruccion para copiar el cPRS en un registro

	lsl r9, #6					@; desplazamos 6 posiciones el zocalo que es lo mismo que multiplicarlo por 64
	add r8, r9, #0xc			@; Sumamos 12 para ir al campo "Status", el cuarto campo

	str r10, [r11, r8]			@; guardar el CPSR en en el campo Status de _gd_psv[z]
	
	@; guardamos el puntero de la pila en un registro libre antes de cambiar de modo y modificar el sp
	
	mov r8, sp					@; guardamos el SP_irq en el registro r8
	
	@;cambiamos el modo de ejecucion y en caso de ser usuario lo cambiamos a sistema
 
	mrs r10, SPSR				@; SPSR contiene el estado anterior del proceso
	and r9, r10, #0x1f			@; solo necesitamos los 5 ultimos bytes
	cmp r9, #0x10				@; comparamos para saber si el estado anterior era de usuario
	bne .LSysMod				@; si es diferente al codigo User pasamos
	mov r9, #0x1f				@; en caso de ser igual lo cambiamos a modo system
.LSysMod:
	mrs r10, CPSR				@; guardamos el valor del cpsr en el registro r10				
	orr r10, r9					@; cambiamos los ultimos 5 bytes
	msr CPSR, r10				@; cambiamos a modo de ejecucion

	@; guardamos en la pila del proceso a desbancar los registro de la pila de dicho proceso
	
	ldr r9, [r8, #0x3c]			@; cargamos el registro r14 de la IRQ para guardarlos en la pila de procesos
	push {r9}
	ldr r9, [r8, #0x38]			@; cargamos el registro r12
	push {r9}
	ldr r9, [r8, #0xc]			@; cargamos el registro r11
	push {r9}
	ldr r9, [r8, #0x8]			@; cargamos el registro r10
	push {r9}
	ldr r9, [r8, #0x4]			@; cargamos el registro r9
	push {r9}
	ldr r9, [r8]				@; cargamos el registro r8
	push {r9}
	ldr r9, [r8, #0x20]			@; cargamos el registro r7
	push {r9}
	ldr r9, [r8, #0x1c]			@; cargamos el registro r6
	push {r9}
	ldr r9, [r8, #0x18]			@; cargamos el registro r5
	push {r9}
	ldr r9, [r8, #0x14]			@; cargamos el registro r4
	push {r9}
	ldr r9, [r8, #0x34]			@; cargamos el registro r3
	push {r9}
	ldr r9, [r8, #0x30]			@; cargamos el registro r2
	push {r9}
	ldr r9, [r8, #0x2c]			@; cargamos el registro r1
	push {r9}
	ldr r9, [r8, #0x28]			@; cargamos el registro r0 
	push {r9}
	
	@; guardamos el puntero de la pila del proceso a desbancar en el campo SP del vector psv de ese proceso
	
	mov r9, #0xf			    @; cargamos en r9 el valor de los 4 bits bajos en 1
	and r9,r7					@; hacemos un and para tener solo el valor de z

	lsl r9, #6					@; desplazamos 6 posiciones el zocalo que es lo mismo que multiplicarlo por 64				
	add r8, r9, #0x8			@; la direccion en la que tenemos que guardar es el (zocalox64) + 8

	str sp, [r11, r8]			@; guardamos el sp del proceso en el campo sp del psv
	
	@; volvemos al modo ejecucion IRQ
	
	mov r8, #0x12				@; guardamos en r8 el modo IRQ
	and r10, #0xffffffe0		@; guardamos los flags de la irq para solo cambiar los 5 ultimos bits
	orr r10, r8
	msr CPSR, r10				@; cambiamos a modo irq
	
	pop {r8-r11, pc}
	

	@; Rutina para restaurar el estado del siguiente proceso en la cola de READY
	@;Parámetros
	@; R4: dirección _gd_nReady
	@; R5: número de procesos en READY
	@; R6: dirección _gd_pidz
_gp_restaurarProc:
	push {r8-r11, lr}
	
	@; decrementamos el contador de procesos pendientes
	
	sub r5, #1					@; decrementamos el contador de procesos de READY
	str r5, [r4]				@; guardamos el nready en su variable
	
	@; recuperamos la primera posicion de la cola de Ready y desplazamos
    @; el resto de posiciones para volver a colocar una en la primera posicion
	
	ldr r8, =_gd_qReady			@; guardamos la direccion de la cola de ready
	ldrb r9, [r8]				@; guardamos en r9 el primer zocalo de la cola de ready
	mov r10, #0
.Lfor:
	cmp r10, r5					@; hacemos un for para desplazar la cola de ready
	bge .LfinFor
	add r10, #1			
	ldrb r11, [r8, r10]			@; guardamos el zocalo la siguente direccion en r11
	sub r10, #1
	strb r11, [r8, r10]			@; guardamos en la anterior direccion el zocalo
	add r10, #1
	b .Lfor
.LfinFor:
	mov r11, #0	
	strb r11, [r8, r5]			@; guardamos un 0 en la ultima posicion 

	@; obtenemos el identificador del proceso a restaurar del campo PID del vector psv de ese proceso
	
	ldr r8, =_gd_psv

	mov r11, r9, lsl #6			@; desplazamos 6 posiciones el zocalo que es lo mismo que multiplicarlo por 64

	str r10, [r8, r11]			@; obtenemos el campo pid del proceso z del psv
	
	@; construimos el valor combinado PIDz
	
	lsl r10, #0x4				@; desplazamos el pid a la izquierda 4 posiciones para hacer sitio al num zocalo
	orr r10, r9					@; construim el PIDz
	str r10, [r6]				@; r10=>_gd_pidz
	
	@; recuperamos el pc buscando en el campo PC del vector psv de ese zocalo
	@; le sumamos 4 bytes para compensar el calculo de retorno que realiza la bios
	@; y lo copiamos en la pila de la IRQ
	
	add r11, #0x4				@; zocalo+4 para acceder al campo PC
	ldr r10, [r8, r11]			@; guardamos en r10 el campo PC 
	mov r8, sp					@; guardamos SP en r8 para el calculo siguiente
	mov r11, #0x3c				@; r11<=60
	str r10, [r8, r11]			@; guardamos en PC en SP_irq + 60
	
	@; recuperamos el CPSR del campo Status del vector psv de ese proceso y lo copiamos en el SPSR de la IRQ
	

	mov r11, r9, lsl #6			@; desplazamos 6 posiciones el zocalo que es lo mismo que multiplicarlo por 64

	add r11, #0xc				@; sumamos 12 para acceder al campo Status
	ldr r8, =_gd_psv			@; r8<=_gd_psv
	ldr r10, [r8, r11]			@; guardamos en r10 el valor del campo Status
	and r11, r10, #0x1f
	msr SPSR, r10				@; guardamos CPSR (Status) en SPSR_irq
	
	@; guardamos el valor del SP de la IRQ en un registro
	
	mov r8, sp					@; guardamos SP_irq
	
	@;cambiamos el modo de ejecucion y en caso de ser usuario lo cambiamos a sistema
	
	cmp r11, #0x10				@; comparamos para saber si el estado anterior era de usuario
	bne .LSysMod2				@; si es diferente al codigo User pasamos
	mov r11, #0x1f				@; en caso de ser igual lo cambiamos a modo system
.LSysMod2:	
	mrs r10, CPSR				@; obtenemos CPSR para obtener los FLAGS actuales
	orr r10, r11				@; cambiamos los ultimos 5 bytes
	msr CPSR, r10				@; cambiamos a modo de ejecucion 

	@; recuperamos el puntero de la pila del proceso a restaurar del vector psv
	@; de ese proceso
	

	ldr r10, =_gd_psv			@; r10<=_gd_psv			
	lsl r9, #6					@; desplazamos 6 posiciones el zocalo que es lo mismo que multiplicarlo por 64
	add r11, r9, #0x8			@; Offset del campo SP

	ldr r13, [r10, r11]			@; r13<= campo SP
	
	@; desapilar el valor de los registros de la pila del proceso a la pila IRQ
	
	pop {r11}					@; empezamos a desapilar la pila del proceso a restaurar
	str r11, [r8, #0x28]		@; guardamos el registro r0
	pop {r11}
	str r11, [r8, #0x2c]		@; guardamos el registro r1
	pop {r11}
	str r11, [r8, #0x30]		@; guardamos el registro r2
	pop {r11}
	str r11, [r8, #0x34]		@; guardamos el registro r3
	pop {r11}
	str r11, [r8, #0x14]		@; guardamos el registro r4
	pop {r11}
	str r11, [r8, #0x18]		@; guardamos el registro r5
	pop {r11}
	str r11, [r8, #0x1c]		@; guardamos el registro r6
	pop {r11}
	str r11, [r8, #0x20]		@; guardamos el registro r7
	pop {r11}
	str r11, [r8, #0x0]			@; guardamos el registro r8
	pop {r11}
	str r11, [r8, #0x4]			@; guardamos el registro r9
	pop {r11}
	str r11, [r8, #0x8]			@; guardamos el registro r10
	pop {r11}
	str r11, [r8, #0xc]			@; guardamos el registro r11
	pop {r11}
	str r11, [r8, #0x38]		@; guardamos el registro r12
	pop {r11}					
	mov r14, r11				@; guardamos el registro r14
	
	@; volvemos al modo IRQ
	
	mov r11, #0x12
	mrs r10, CPSR
	and r10, #0xffffffe0
	orr r10, r11
	msr CPSR, r10
	
	pop {r8-r11, pc}


	@; Rutina para actualizar la cola de procesos retardados, poniendo en
	@; cola de READY aquellos cuyo número de tics de retardo sea 0
_gp_actualizarDelay:
	push {lr}


	pop {pc}
	
	
	.global _gp_numProc
	@;Resultado
	@; R0: número total de procesos cargados en el sistema
_gp_numProc:
	push {r1-r2, lr}
	mov r0, #1				@; contar siempre 1 proceso en RUN
	ldr r1, =_gd_nReady
	ldr r2, [r1]			@; R2 = número de procesos en cola de READY
	add r0, r2				@; añadir procesos preparados
	ldr r1, =_gd_nDelay
	ldr r2, [r1]			@; R2 = número de procesos en cola de DELAY
	add r0, r2				@; añadir procesos retardados
	pop {r1-r2, pc}
	

	.global _gp_crearProc
	@; prepara un proceso para ser ejecutado, creando su entorno de ejecución y
	@; colocándolo en la cola de READY
	@;Parámetros
	@; R0: intFunc funcion,
	@; R1: int zocalo,
	@; R2: char *nombre
	@; R3: int argumen
	@;Resultado
	@; R0: 0 si no hay problema, >0 si no se puede crear el proceso
_gp_crearProc:
	push {r1-r8, lr}
	
	@; comprobamos si el zocalo es el reservado para el sistema operativo 
	@; o si ya esta ocupado por otro proceso
	
	cmp r1, #0				@; en caso de que z sea 0 rechazamos la llamada
	beq	.LcrearEnd
	ldr r8, =_gd_psv		@; cargamos la direccion del psv en r8
	mov r3, #0x3c			@; guardamos el numero 60 en r8 para multiplicarlo por el zocalo
	mul r7, r1, r3	 		@; multiplicamos el zocalo por 60
	ldr r4, [r8, r7]		@; r4= PID del _gd_psv[z]
	cmp r4, #0				@; en caso de que pid sea diferente a 0 saltamos al final
	bne .LcrearEnd
	
	@; incrementamos la variable pidCount y la guardamos en el campo PID del vector psv de ese proceso
	
	ldr r5,  =_gd_pidCount	@; cargamos la direccion de pidCount en r5
	ldr r6, [r5]			@; cargamos el valor de pidCount en r6
	add r6, #1				@; incrementamos pidCount
	str r6, [r5]			@; guardamos el incremento en su variable
	str r6, [r8, r7]		@; PId de _gd_psv[z]= pidCount
	
	@; guardamos la direccion de la rutina inicial en el campo PC en el vector psv de ese proceso
	
	add r3, r7, #0x4		@; direccion del PC del psv
	str r0, [r8, r3]		@; guardamos la funcion en el campo PC en el psv de z
	
	@; guardamos los 4 primeros caracteres en el campo keyName en el vector psv de ese proceso
	
	add r3, r7, #0x10		@; direccion del keyName del psv
	str r2, [r8,r3]			@; guardamos el nombre de la funcion en el campo keyName de psv
	
	@; calculamos la direccion base de la pila del proceso
	
	mov r3, r1, lsl #9		@; calculamos el offset del _gd_stacks
	
	@; guardamos en la pila del proceso el valor inicial de los registros
	

	ldr r5, =_gd_stacks		@; guardamos la direccion del vector de pilas
	
	sub r3, #0x4			@; como se trata del vector de pilas de procesos tenemos que apilar desdel final de su offset
	ldr r6, =_gp_terminarProc 	@; cargamos la direccion de terminarproc
	str r6, [r5, r3]		@; guardamos la direccion de terminarproc en el registro 14 del vector de pilas
	mov r6, #0
	mov r8, #0
.Lfor2:						@; bucle para inicializar los registros 0-12
	cmp r8, #0xd
	bge .Lfinfor2
	sub r3, #0x4			@; restamos 4 para guardar en cada posicion siguiente
	str r6, [r5, r3]		@; guardamos 0 en el registro 12 del vector de pilas
	add r8, #1
	b .Lfor2
.Lfinfor2:
	
	ldr r8, =_gd_psv		@; cargamos la direccion del psv en r8
	
	@; guardamos el valor del puntero de la pila en el campo SP del vector psv de este proceso
	
	add r7, #0x8			@; en el registro 7 teniamos guardada la direccion del psv de z solo hay que sumar 8 para escribir en SP
	add r5, r3
	str r5, [r8, r7]		@; guardamos el registro 13 en el campo SP del psv
	
	@; guardamos el valor 10 que corresponde al modo ejecucion en el campo Status del vector psv de este proceso
	
	mov r6, #0x10			@; codigo del modo usuario
	add r7, #0x4		
	str r6, [r8, r7]		@; guardamos el codigo en el campo status del psv
	
	@; inicializamos otros valores del vector psv de este proceso
	
	mov r6, #0		
	add r7, #0x4
.Lfor3:	
	cmp r5, #0xb			 @; bucle en el que inicializamos las variables de psv
	bge .Lfinfor3
	add r7, #0x4
	str r6, [r8, r7]		@; de numticks a pchar
	add r5, #1
	b .Lfor3
.Lfinfor3:
	
	@; guardamos el zocalo en la ultima posicion de la cola de Ready e incrememtamos la variable de nReady
	
	ldr r7, =_gd_nReady		@; guardamos la direccion de la variable numReady
	ldr r8, =_gd_qReady		@; guardamos la direccion de la cola de ready
	ldr r6, [r7]			@; cargamos el valor de la variable numReady
	strb r1, [r8, r6]		@; guardamos el zocalo en la ultima posicion de la cola de ready
	add r6, #1				@; incrementamos la variable numReady
	str r6, [r7]			@; guardamos la r6 en la variable numReady
	mov r0, #0
.LcrearEnd:

	pop {r1-r8, pc}



	@; Función para terminar un proceso de usuario:
	@; borra el PID del PSB del zócalo actual, para indicar que esa entrada
	@; del vector _gd_psv está libre; también elimina el PID de la variable
	@; _gd_pidz (sin modificar el número de zócalo), para que el procedimiento
	@; de intercambio de procesos NO salve el estado del proceso terminado.
_gp_terminarProc:
	ldr r0, =_gd_pidz
	ldr r1, [r0]			@; R1 = valor actual de PID + zócalo
	and r1, r1, #0xf		@; R1 = zócalo del proceso a terminar
	bl _gp_inhibirIRQs
	str r1, [r0]			@; guardar zócalo con PID = 0, para no salvar estado			
	ldr r2, =_gd_psv
	mov r10, #92
	mul r11, r1, r10
	add r2, r11				@; R2 = dirección base _gd_psv[zocalo]
	mov r3, #0
	str r3, [r2]			@; borrar PID del bloque de estado del proceso
	str r3, [r2, #20]		@; borrar porcentaje de USO de la CPU
	ldr r0, =_gd_sincMain
	ldr r2, [r0]			@; R2 = valor actual de la variable de sincronismo
	mov r3, #1
	mov r3, r3, lsl r1		@; R3 = máscara con bit correspondiente al zócalo
	orr r2, r3
	str r2, [r0]			@; actualizar variable de sincronismo
	bl _gp_desinhibirIRQs
.LterminarProc_inf:
	bl _gp_WaitForVBlank	@; pausar procesador
	b .LterminarProc_inf	@; asegurar el cambio de contexto


	.global _gp_matarProc
	@; Función para destruir un proceso de usuario:
	@; borra el PID del PSB del zócalo indiciado por parámetro, para indicar
	@; que esa entrada del vector _gd_psv está libre; elimina el índice de
	@;  zócalo de la cola de READY o de la cola de DELAY, según el caso
	@; Parámetros:
	@;	R0:	zócalo del proceso a matar (entre 1 y 15).
_gp_matarProc:
	push {lr} 


	pop {pc}

	
	.global _gp_retardarProc
	@; retarda la ejecución de un proceso durante cierto número de segundos,
	@; colocándolo en la cola de DELAY
	@;Parámetros
	@; R0: int nsec
_gp_retardarProc:
	push {lr}


	pop {pc}


	.global _gp_inihibirIRQs
	@; pone el bit IME (Interrupt Master Enable) a 0, para inhibir todas
	@; las IRQs y evitar así posibles problemas debidos al cambio de contexto
_gp_inhibirIRQs:
	push {r11-r12, lr}
	mov	r12, #0x4000000
	add	r12, r12, #0x208	@; R12 = base registros de control de interrupciones
	mov r11, #0
	strh r11, [r12]			@; REG_IME = 0 (interrupciones no permitidas)
	pop {r11-r12, pc}


	.global _gp_desinihibirIRQs
	@; pone el bit IME (Interrupt Master Enable) a 1, para desinhibir todas
	@; las IRQs
_gp_desinhibirIRQs:
	push {r11-r12, lr}
	mov	r12, #0x4000000
	add	r12, r12, #0x208	@; R12 = base registros de control de interrupciones
	mov r11, #1
	strh r11, [r12]			@; REG_IME = 1 (interrupciones permitidas)
	pop {r11-r12, pc}


.end

