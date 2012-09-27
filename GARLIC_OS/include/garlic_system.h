/*------------------------------------------------------------------------------

	"garlic_system.h" : definiciones de las variables globales y funciones
					 del sistema operativo GARLIC (versi�n 0.5)

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

extern int _gd_seed;		// Semilla para generaci�n de n�meros aleatorios
							// (tiene que ser diferente de cero)
	
extern int _gd_nReady;		// N�mero de procesos en cola de READY (de 0 a 16)

extern char _gd_qReady[16];	// Cola de READY (procesos preparados) : vector
							// ordenado con _gd_nReady entradas, conteniendo
							// los identificadores (0-15) de los z�calos de los
							// procesos (m�x. 15 procesos + sistema operativo)

typedef struct				// Estructura del bloque de estado de un proceso
{							// (PSB: Process Status Block)
	int PID;				//	identificador del proceso (Process IDentifier)
	int PC;					//	contador de programa (Program Counter)
	int SP;					//	putero �ltima entrada de la pila (Stack Pointer)
	int Status;				//	estado del procesador (CPSR)
	int keyName;			//	nombre en clave del proceso (cuatro chars)
	int numTicks;			//	n�mero total de ciclos de ejecuci�n
	int workTicks;			//	contador de ciclos de trabajo (uso de CPU)
	int pControl;			//	control de escritura en ventana del z�calo
							//		16 bits altos: n�mero de l�nea (0-23)
							//		16 bits bajos: caracteres pendientes (0-32)
	char pChars[32];		//	vector de 32 caracteres pendientes de escritura
} PACKED garlicPSB;

extern garlicPSB _gd_psv[16];// vector con los PSB de cada z�calo

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

/* _gg_escribir: escribe una cadena de caracteres en el z�calo indicado;
	Par�metros:
		mensaje	->	cadena de c�digos ASCII, terminada con centinela '\0';
					admite '\n' (salto de l�nea), '\t' (tabulador, 4 espacios)
					y c�digos ASCII entre 32 y 159 (los c�digos mayores de 126
					corresponden a caracteres gr�ficos)
		z�calo	->	n�mero de z�calo (de 0 a 15)
*/
extern void _gg_escribir(char *mensaje, int zocalo);



#endif // _GARLIC_SYSTEM_h_
