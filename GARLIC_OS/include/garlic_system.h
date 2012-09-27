/*------------------------------------------------------------------------------

	"garlic_system.h" : definiciones de las variables globales y funciones
					 del sistema operativo GARLIC (versión 0.5)

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

extern int _gd_seed;		// Semilla para generación de números aleatorios
							// (tiene que ser diferente de cero)
	
extern int _gd_nReady;		// Número de procesos en cola de READY (de 0 a 16)

extern char _gd_qReady[16];	// Cola de READY (procesos preparados) : vector
							// ordenado con _gd_nReady entradas, conteniendo
							// los identificadores (0-15) de los zócalos de los
							// procesos (máx. 15 procesos + sistema operativo)

typedef struct				// Estructura del bloque de estado de un proceso
{							// (PSB: Process Status Block)
	int PID;				//	identificador del proceso (Process IDentifier)
	int PC;					//	contador de programa (Program Counter)
	int SP;					//	putero última entrada de la pila (Stack Pointer)
	int Status;				//	estado del procesador (CPSR)
	int keyName;			//	nombre en clave del proceso (cuatro chars)
	int numTicks;			//	número total de ciclos de ejecución
	int workTicks;			//	contador de ciclos de trabajo (uso de CPU)
	int pControl;			//	control de escritura en ventana del zócalo
							//		16 bits altos: número de línea (0-23)
							//		16 bits bajos: caracteres pendientes (0-32)
	char pChars[32];		//	vector de 32 caracteres pendientes de escritura
} PACKED garlicPSB;

extern garlicPSB _gd_psv[16];// vector con los PSB de cada zócalo

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

/* _gg_escribir: escribe una cadena de caracteres en el zócalo indicado;
	Parámetros:
		mensaje	->	cadena de códigos ASCII, terminada con centinela '\0';
					admite '\n' (salto de línea), '\t' (tabulador, 4 espacios)
					y códigos ASCII entre 32 y 159 (los códigos mayores de 126
					corresponden a caracteres gráficos)
		zócalo	->	número de zócalo (de 0 a 15)
*/
extern void _gg_escribir(char *mensaje, int zocalo);



#endif // _GARLIC_SYSTEM_h_
