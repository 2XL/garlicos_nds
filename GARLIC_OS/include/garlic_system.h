/*------------------------------------------------------------------------------

	"garlic_system.h" : definiciones de las variables globales y funciones
					 del sistema operativo GARLIC (versi�n 1.0)

------------------------------------------------------------------------------*/
#ifndef _GARLIC_SYSTEM_h_
#define _GARLIC_SYSTEM_h_


//------------------------------------------------------------------------------
//	Variables globales (garlic_dtcm.s)
//------------------------------------------------------------------------------

extern int _gd_pidz;		// Identificador de proceso (PID) + z�calo
							// (PID en 28 bits altos, z�calo en 4 bits bajos,
							// cero si se trata del propio sistema operativo)
							
extern int _gd_pidCount;	// Contador de PIDs: se incrementa cada vez que
							// se crea un nuevo proceso (max. 2^28)
							
extern int _gd_tickCount;	// Contador de tics: se incrementa cada IRQ_VBL, y
							// determina el final del quantum (aprox. 16.7 ms)

extern int _gd_sincMain;	// Sincronismos con programa principal:
							// bit 0 = 1 indica si se ha acabado de calcular el
							// 				el uso de la CPU,
							// bits 1-15 = 1 indican si el proceso del z�calo
							//				asociado han terminado.

extern int _gd_seed;		// Semilla para generaci�n de n�meros aleatorios
							// (tiene que ser diferente de cero)
	
extern int _gd_nReady;		// N�mero de procesos en cola de READY (de 0 a 16)

extern char _gd_qReady[16];	// Cola de READY (procesos preparados) : vector
							// ordenado con _gd_nReady entradas, conteniendo
							// los identificadores (0-15) de los z�calos de los
							// procesos (m�x. 15 procesos + sistema operativo)

extern int _gd_nDelay;		// N�mero de procesos en cola de DELAY (de 0 a 16)

extern int _gd_qDelay[16];	// Cola de DELAY (procesos retardados) : vector
							// con _gd_nDelay entradas, conteniendo los
							// identificadores de los z�calos (8 bits altos)
							// m�s el n�mero de segundos restantes (16 bits
							// bajos) para desbloquear el proceso

typedef struct				// Estructura del bloque de estado de un proceso
{							// (PSB: Process Status Block)
	int PID;				//	identificador del proceso (Process IDentifier)
	int PC;					//	contador de programa (Program Counter)
	int SP;					//	putero �ltima entrada de la pila (Stack Pointer)
	int Status;				//	estado del procesador (CPSR)
	int keyName;			//	nombre en clave del proceso (cuatro chars)
	int workTicks;			//	contador de ciclos de trabajo (24 bits bajos)
							//		8 bits altos: uso de CPU (%)
	int pControl;			//	control de escritura en ventana del z�calo
							//		16 bits altos: n�mero de l�nea (0-23)
							//		16 bits bajos: caracteres pendientes (0-32)
	short pChars[32];		//	vector de 32 caracteres pendientes de escritura
							//		16 bits por entrada, indicando n�mero de
							//		baldosa correspondiente al caracter+color
} PACKED garlicPSB;

extern garlicPSB _gd_psv[16];	// vector con los PSB de cada z�calo

extern int _gd_stacks[15*128];	// vector con las pilas de los procesos activos



//------------------------------------------------------------------------------
//	Funciones internas del api del sistema (garlic_itcm_api.s);
//				para m�s informaci�n consultar el fichero (GARLIC_API.h)
//------------------------------------------------------------------------------

/* _gi_num2str:	convertir par�metro num en una representaci�n en c�digos ASCII
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


/* _gp_numProc:	devuelve el n�mero de procesos cargados en el sistema
				actualmente, incluyendo el proceso en RUN, los procesos
				en READY y los procesos bloqueados */
extern int _gp_numProc();


/* _gp_crearProc:	prepara un proceso para ser ejecutado, creando su entorno
				de ejecuci�n y coloc�ndolo en la cola de READY;
	Par�metros:
		funcion	->	direcci�n de memoria de entrada al c�digo del proceso
		zocalo	->	identificador del z�calo (0 - 15)
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
					del sistema, efectuando la relocataci�n de las referencias a 
					los s�mbolos del programa;
	Par�metors:
		keyName ->	string de 4 caracteres con el nombre en clave del progrma
	Resultado:
		!= 0	->	direcci�n de inicio del programa (intFunc)
		== 0	->	no se ha podido cargar el programa
*/
extern intFunc _gm_cargarPrograma(char *keyName);



//------------------------------------------------------------------------------
//	Funciones de gr�ficos (garlic_graf.c)
//------------------------------------------------------------------------------

/* _gg_iniGraf: inicializa el entorno gr�fico para GARLIC */
extern void _gg_iniGraf();


/* Escribe los campos b�sicos de una linea de la tabla correspondiente al
		z�calo indicado por par�metro con el color especificado; los campos
		son: n�mero de z�calo, PID, keyName y direcci�n inicial */
extern void _gg_escribirLineaTabla(int z, int color);


/* Dibuja las separaciones de la linea de la tabla correspondientes al z�calo
		que se pasa por par�metro, con el color especificado */
extern void _gg_dibujarSeparadoresTabla(int z, int color);


/* Dibujar el marco correspondiente al z�calo que se pasa por par�metro, seg�n
	el color especificado */
extern void _gg_dibujarMarco(int z, int color);


/* _gg_escribir: escribe una cadena de caracteres en el z�calo indicado;
	Par�metros:
		mensaje	->	cadena de c�digos ASCII, terminada con centinela '\0';
					admite '\n' (salto de l�nea), '\t' (tabulador, 4 espacios)
					y c�digos ASCII entre 32 y 159 (los c�digos mayores de 126
					corresponden a caracteres gr�ficos)
		color	->	n�mero de color del texto (de 0 a 3)
		z�calo	->	n�mero de z�calo (de 0 a 15)
*/
extern void _gg_escribir(char *mensaje, int color, int zocalo);


/* _gg_escribiCarr: escribe un car�cter (baldosa) en la posici�n indicada;
	Par�metros:
		vx		->	coordenada x de ventana (0..31)
		vy		->	coordenada y de ventana (0..23)
		caracter->	c�digo del car�cter, como n�mero de baldosa (0..127)
		color	->	n�mero de color del texto (de 0 a 3)
		z�calo	->	n�mero de z�calo (de 0 a 15)
*/
extern void _gg_escribirCar(int vx, int vy, char c, int color, int zocalo);


/* _gg_escribirMat: escribir una matriz de 8x8 car�cteres en la posici�n indicada;
	Par�metros:
		vx		->	coordenada x inicial de ventana (0..31)
		vy		->	coordenada y inicial de ventana (0..23)
		m		->	matriz 8x8 de c�digos ASCII
		color	->	n�mero de color del texto (de 0 a 3)
		z�calo	->	n�mero de z�calo (de 0 a 15)
*/
extern void _gg_escribirMat(int vx, int vy, char m[][8], int color, int zocalo);



//------------------------------------------------------------------------------
//	Funciones de soporte al sistema (garlic_itcm_sys.s)
//------------------------------------------------------------------------------

/* Borra el contenido de la ventana correspondiente al z�calo que se pasa por
		par�metro, as� como el campo _gd_psv[z].pControl;
	Par�metros:
		zocalo 	->	n�mero de z�calo
		modo 	->	modo de las ventanas (0 -> 4 ventanas, 1 -> 16 ventanas)
*/
extern void _gs_borrarVentana(int zocalo, int modo);


/* Pintar las franjas verticales correspondientes a los trozos de memoria 
		indicados por par�metro, con el color tambi�n indicado por par�metro.
	Par�metros:
		tile_base	->	la base de las baldosas correspondientes a las franjas
		index_ini	->	el �ndice inicial de las franjas
		num_franjas	->	el n�mero de franjas a pintar
		color		->	el color que hay que utilizar */
extern void _gs_pintarFranjas(unsigned int tile_base, unsigned short index_ini,
							unsigned short num_franjas, unsigned char color);


/* Escribe un string (terminado con centinela cero) a partir de la posici�n
		indicada por par�metros (fil, col), con el color especificado, en la
		pantalla secundaria */
extern void _gs_escribirStringSub(char *string, int fil, int col, int color);


/* _gp_rsiTIMER0:	manejador de interrupciones del TIMER0 de Garlic */
extern void _gs_rsiTIMER0();


/* _gs_rsiTIMER1:	manejador de interrupciones del TIMER1 de Garlic */
extern void _gs_rsiTIMER1();




#endif // _GARLIC_SYSTEM_h_
