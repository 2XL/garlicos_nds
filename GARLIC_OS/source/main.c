/*------------------------------------------------------------------------------

	"main.c" : fase 1 / master

	Programa de control del sistema operativo GARLIC, versión 0.5

------------------------------------------------------------------------------*/
#include <nds.h>
#include <filesystem.h>
#include <stdlib.h>

#include <garlic_system.h>	// definición de funciones y variables de sistema

extern int * punixTime;		// puntero a zona de memoria con el tiempo real


/* Inicializaciones generales del sistema Garlic */
//------------------------------------------------------------------------------
void inicializarSistema() {
//------------------------------------------------------------------------------
	
	_gg_iniGraf();			// inicializar gráficos
	
	_gd_seed = *punixTime;	// inicializar semilla para números aleatorios con
	_gd_seed <<= 16;		// el valor de tiempo real UNIX, desplazado 16 bits
	
	irqInitHandler(_gp_IntrMain);	// inicializar IRQs
	irqSet(IRQ_VBLANK, _gp_rsiVBL);
	irqEnable(IRQ_VBLANK);
	
	_gd_psv[0].keyName = 0x4C524147;	// "GARL"

	if (!nitroFSInit()) {
		_gg_escribir("ERROR: ¡no se puede utilizar NitroFS!", 0);
		exit(0);
	}
}


//------------------------------------------------------------------------------
int main(int argc, char **argv) {
//------------------------------------------------------------------------------
	intFunc start;
	char string[8];

	inicializarSistema();
	
	_gg_escribir("********************************", 0);
	_gg_escribir("*                              *", 0);
	_gg_escribir("* Sistema Operativo GARLIC 0.5 *", 0);
	_gg_escribir("*                              *", 0);
	_gg_escribir("********************************", 0);
	_gg_escribir("*** Inicio fase 1\n", 0);

	_gg_escribir("*** Carga de programa HOLA.elf\n", 0);
	start = _gm_cargarPrograma("HOLA");
	if (start)
	{	
		_gg_escribir("*** Pusle \'START\' ::\n\n", 0);
		while(1) {
			_gp_WaitForVBlank();
			scanKeys();
			if (keysDown() & KEY_START) break;
		}
		
		_gp_crearProc(start, 3, "HOLA");
		
		while (_gp_numProc() > 1)
		{						// esperar a que termine el proceso pendiente
			_gg_escribir("*** Test \t", 0);
			_gi_num2str(string, 5, _gd_tickCount);
			string[4] = '\t';
			_gi_num2str(&string[5], 2, _gp_numProc());
			string[6] = '\n'; string[7] = 0;
			_gg_escribir(string, 0);
		}
	} else
		_gg_escribir("*** Programa NO cargado\n", 0);

	_gg_escribir("*** Final fase 1\n", 0);
	
	while(1) {
		_gp_WaitForVBlank();
	}							// parar el procesador en un bucle infinito
	return 0;
}


