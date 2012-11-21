/*------------------------------------------------------------------------------

	"garlic_system.h" : definiciones de las variables globales y funciones
					 del sistema operativo GARLIC (versión 1.0)

------------------------------------------------------------------------------*/
#ifndef _GARLIC_SYSTEM_h_
#define _GARLIC_SYSTEM_h_


//------------------------------------------------------------------------------
//	Variables globales (garlic_dtcm.s)
//------------------------------------------------------------------------------

extern int _gd_pidz;		// Identificador de proceso (PID) + zócalo
							// (PID en 28 bits altos, zócalo en 4 bits bajos,
							// cero si se trata del propio sistema operativo)
							
extern int _gd_pidCount;	// Contador de PIDs: se incrementa cada vez que
							// se crea un nuevo proceso (max. 2^28)
							
extern int _gd_tickCount;	// Contador de tics: se incrementa cada IRQ_VBL, y
							// determina el final del quantum (aprox. 16.7 ms)

extern int _gd_sincMain;	// Sincronismos con programa principal:
							// bit 0 = 1 indica si se ha acabado de calcular el
							// 				el uso de la CPU,
							// bits 1-15 = 1 indican si el proceso del zócalo
							//				asociado han terminado.

extern int _gd_seed;		// Semilla para generación de números aleatorios
							// (tiene que ser diferente de cero)
	
extern int _gd_nReady;		// Número de procesos en cola de READY (de 0 a 16)

extern char _gd_qReady[16];	// Cola de READY (procesos preparados) : vector
							// ordenado con _gd_nReady entradas, conteniendo
							// los identificadores (0-15) de los zócalos de los
							// procesos (máx. 15 procesos + sistema operativo)

extern int _gd_nDelay;		// Número de procesos en cola de DELAY (de 0 a 16)

extern int _gd_qDelay[16];	// Cola de DELAY (procesos retardados) : vector
							// con _gd_nDelay entradas, conteniendo los
							// identificadores de los zócalos (8 bits altos)
							// más el número de segundos restantes (16 bits
							// bajos) para desbloquear el proceso

typedef struct				// Estructura del bloque de estado de un proceso
{							// (PSB: Process Status Block)
	int PID;				//	identificador del proceso (Process IDentifier)
	int PC;					//	contador de programa (Program Counter)
	int SP;					//	putero última entrada de la pila (Stack Pointer)
	int Status;				//	estado del procesador (CPSR)
	int keyName;			//	nombre en clave del proceso (cuatro chars)
	int workTicks;			//	contador de ciclos de trabajo (24 bits bajos)
							//		8 bits altos: uso de CPU (%)
	int pControl;			//	control de escritura en ventana del zócalo
							//		16 bits altos: número de línea (0-23)
							//		16 bits bajos: caracteres pendientes (0-32)
	short pChars[32];		//	vector de 32 caracteres pendientes de escritura
							//		16 bits por entrada, indicando número de
							//		baldosa correspondiente al caracter+color
} PACKED garlicPSB;

extern garlicPSB _gd_psv[16];	// vector con los PSB de cada zócalo

extern int _gd_stacks[15*128];	// vector con las pilas de los procesos activos



//------------------------------------------------------------------------------
//	Funciones internas del api del sistema (garlic_itcm_api.s);
//				para más información consultar el fichero (GARLIC_API.h)
//------------------------------------------------------------------------------

/* _gi_num2str:	convertir parámetro num en una representación en códigos ASCII
				sobre el vector de caracteres numstr */
extern void _gi_num2str(char * numstr, unsigned int length, unsigned int num);



//------------------------------------------------------------------------------
//	Funciones de sistema (garlic_itcm_proc.s)
//------------------------------------------------------------------------------

typedef int (* intFunc)(void);

/* _gp_WaitForVBlank:	sustituto de swiWaitForVBlank para procesos de Garlic */
extern void _gp_WaitForVBlank();

/* _gp_IntrMain:	manejador principal de interrupciones del sistema Garlic */
extern void _gp_IntrMain();

/* _gp_rsiVBL:	manejador de interrupciones VBL (Vertical BLank) de Garlic */
extern void _gp_rsiVBL();


/* _gp_numProc:	devuelve el número de procesos cargados en el sistema
				actualmente, incluyendo el proceso en RUN, los procesos
				en READY y los procesos bloqueados */
extern int _gp_numProc();


/* _gp_crearProc:	prepara un proceso para ser ejecutado, creando su entorno
				de ejecución y colocándolo en la cola de READY;
	Parámetros:
		funcion	->	dirección de memoria de entrada al código del proceso
		zocalo	->	identificador del zócalo (0 - 15)
		nombre	->	nombre en clave del programa (4 caracteres ASCII)
	Resultado:	0 si no hay problema, >0 si no se puede crear el proceso
*/
extern int _gp_crearProc(intFunc funcion, int zocalo, char *nombre);



//------------------------------------------------------------------------------
//	Funciones de memoria (garlic_mem.c)
//------------------------------------------------------------------------------

/* _gm_cargarPrograma: busca un fichero de nombre "(keyName).elf" dentro del
					directorio "/Programas/" del sistema de ficheros Nitro, y
					carga los segmentos de programa en una zona libre de memoria
					del sistema, efectuando la relocatación de las referencias a 
					los símbolos del programa;
	Parámetors:
		keyName ->	string de 4 caracteres con el nombre en clave del progrma
	Resultado:
		!= 0	->	dirección de inicio del programa (intFunc)
		== 0	->	no se ha podido cargar el programa
*/
extern intFunc _gm_cargarPrograma(char *keyName);



//------------------------------------------------------------------------------
//	Funciones de gráficos (garlic_graf.c)
//------------------------------------------------------------------------------

/* _gg_iniGraf: inicializa el entorno gráfico para GARLIC */
extern void _gg_iniGraf();


/* Escribe los campos básicos de una linea de la tabla correspondiente al
		zócalo indicado por parámetro con el color especificado; los campos
		son: número de zócalo, PID, keyName y dirección inicial */
extern void _gg_escribirLineaTabla(int z, int color);


/* Dibuja las separaciones de la linea de la tabla correspondientes al zócalo
		que se pasa por parámetro, con el color especificado */
extern void _gg_dibujarSeparadoresTabla(int z, int color);


/* Dibujar el marco correspondiente al zócalo que se pasa por parámetro, según
	el color especificado */
extern void _gg_dibujarMarco(int z, int color);


/* _gg_escribir: escribe una cadena de caracteres en el zócalo indicado;
	Parámetros:
		mensaje	->	cadena de códigos ASCII, terminada con centinela '\0';
					admite '\n' (salto de línea), '\t' (tabulador, 4 espacios)
					y códigos ASCII entre 32 y 159 (los códigos mayores de 126
					corresponden a caracteres gráficos)
		color	->	número de color del texto (de 0 a 3)
		zócalo	->	número de zócalo (de 0 a 15)
*/
extern void _gg_escribir(char *mensaje, int color, int zocalo);


/* _gg_escribiCarr: escribe un carácter (baldosa) en la posición indicada;
	Parámetros:
		vx		->	coordenada x de ventana (0..31)
		vy		->	coordenada y de ventana (0..23)
		caracter->	código del caràcter, como número de baldosa (0..127)
		color	->	número de color del texto (de 0 a 3)
		zócalo	->	número de zócalo (de 0 a 15)
*/
extern void _gg_escribirCar(int vx, int vy, char c, int color, int zocalo);


/* _gg_escribirMat: escribir una matriz de 8x8 carácteres en la posición indicada;
	Parámetros:
		vx		->	coordenada x inicial de ventana (0..31)
		vy		->	coordenada y inicial de ventana (0..23)
		m		->	matriz 8x8 de códigos ASCII
		color	->	número de color del texto (de 0 a 3)
		zócalo	->	número de zócalo (de 0 a 15)
*/
extern void _gg_escribirMat(int vx, int vy, char m[][8], int color, int zocalo);



//------------------------------------------------------------------------------
//	Funciones de soporte al sistema (garlic_itcm_sys.s)
//------------------------------------------------------------------------------

/* Borra el contenido de la ventana correspondiente al zócalo que se pasa por
		parámetro, así como el campo _gd_psv[z].pControl;
	Parámetros:
		zocalo 	->	número de zócalo
		modo 	->	modo de las ventanas (0 -> 4 ventanas, 1 -> 16 ventanas)
*/
extern void _gs_borrarVentana(int zocalo, int modo);


/* Pintar las franjas verticales correspondientes a los trozos de memoria 
		indicados por parámetro, con el color también indicado por parámetro.
	Parámetros:
		tile_base	->	la base de las baldosas correspondientes a las franjas
		index_ini	->	el índice inicial de las franjas
		num_franjas	->	el número de franjas a pintar
		color		->	el color que hay que utilizar */
extern void _gs_pintarFranjas(unsigned int tile_base, unsigned short index_ini,
							unsigned short num_franjas, unsigned char color);


/* Escribe un string (terminado con centinela cero) a partir de la posición
		indicada por parámetros (fil, col), con el color especificado, en la
		pantalla secundaria */
extern void _gs_escribirStringSub(char *string, int fil, int col, int color);


/* _gp_rsiTIMER0:	manejador de interrupciones del TIMER0 de Garlic */
extern void _gs_rsiTIMER0();


/* _gs_rsiTIMER1:	manejador de interrupciones del TIMER1 de Garlic */
extern void _gs_rsiTIMER1();




#endif // _GARLIC_SYSTEM_h_
