/*------------------------------------------------------------------------------

	"main.c" : fase 1 / programador M

	Programa de prueba de carga de un fichero ejecutable en formato ELF,
	efectuando la necesaria relocatación y ejecutando su código.

------------------------------------------------------------------------------*/
#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include <stdlib.h>

#include <garlic_system.h>	// definición de funciones y variables de sistema

extern int * punixTime;		// puntero a zona de memoria con el tiempo real


/* Inicializaciones generales del sistema Garlic */
//------------------------------------------------------------------------------
void inicializarSistema() {
//------------------------------------------------------------------------------
	
	consoleDemoInit();		// inicializar console sólo para realizar pruebas
	
	_gd_seed = *punixTime;	// inicializar semilla para números aleatorios con
	_gd_seed <<= 16;		// el valor de tiempo real UNIX, desplazado 16 bits
	
	irqInitHandler(_gp_IntrMain);	// inicializar IRQs
	irqSet(IRQ_VBLANK, _gp_rsiVBL);
	irqEnable(IRQ_VBLANK);
	
	if (!nitroFSInit()) {
		printf("ERROR: ¡no se puede utilizar Nitro File System!");
		exit(0);
	}
}


//------------------------------------------------------------------------------
int main(int argc, char **argv) {
//------------------------------------------------------------------------------
	intFunc start;
	inicializarSistema();
	
	_gp_WaitForVBlank();
	printf("********************************");
	printf("*                              *");
	printf("* Sistema Operativo GARLIC 0.5 *");
	printf("*                              *");
	printf("********************************");
	printf("*** Inicio fase 1_M\n");
	
	printf("*** Carga de programa HOLA.elf\n");
	start = _gm_cargarPrograma("HOLA");
	if (start)
	{	printf("*** Direccion de arranque :\n\t\t%p\n", start);
		printf("*** Pusle \'START\' ::\n\n");
		while(1) {
			_gp_WaitForVBlank();
			scanKeys();
			if (keysDown() & KEY_START) break;
		}
		start();
	} else
		printf("*** Programa NO cargado\n");

	printf("*** Final fase 1_M\n");

	while(1) {
		_gp_WaitForVBlank();
	}							// parar el procesador en un bucle infinito
	return 0;
}



/* rutina de soporte para _gi_print */
//------------------------------------------------------------------------------
void _gg_escribir(char * message, int z) {
//------------------------------------------------------------------------------
	printf("%s", message);	// imprimir toda la línea
}

