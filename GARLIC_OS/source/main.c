/*------------------------------------------------------------------------------

	"main.c" : fase 2 / ProgP

	Programa de control del sistema operativo GARLIC, versión 1.0

------------------------------------------------------------------------------*/
#include <nds.h>
#include <filesystem.h>
#include <stdlib.h>

#include <garlic_system.h>	// definición de funciones y variables de sistema

extern int * punixTime;		// puntero a zona de memoria con el tiempo real

const short divFreq0 = -33513982/1024;		// frecuencia de TIMER0 = 1 Hz


/* función para escribir los porcentajes de uso de la CPU de los procesos de los
		cuatro primeros zócalos, en el caso que la RSI del TIMER0 haya realizado
		el cálculo */
void porcentajeUso()
{
	char string[8];

	if (_gd_sincMain & 1)			// verificar sincronismo de timer0
	{
		_gd_sincMain &= 0xFFFE;			// poner bit de sincronismo a cero
		_gg_escribir("***\t", 0);
		_gi_num2str(string, 4, _gd_psv[0].workTicks >> 24);
		_gg_escribir(string, 0);
		_gg_escribir("%  ", 0);
		_gi_num2str(string, 4, _gd_psv[1].workTicks >> 24);
		_gg_escribir(string, 0);
		_gg_escribir("%  ", 0);
		_gi_num2str(string, 4, _gd_psv[2].workTicks >> 24);
		_gg_escribir(string, 0);
		_gg_escribir("%  ", 0);
		_gi_num2str(string, 4, _gd_psv[3].workTicks >> 24);
		_gg_escribir(string, 0);
		_gg_escribir("%\n", 0);
	}
}


/* Inicializaciones generales del sistema Garlic */
//------------------------------------------------------------------------------
void inicializarSistema() {
//------------------------------------------------------------------------------
	
	_gd_seed = *punixTime;	// inicializar semilla para números aleatorios con
	_gd_seed <<= 16;		// el valor de tiempo real UNIX, desplazado 16 bits
	
	_gd_psv[0].keyName = 0x4C524147;	// "GARL"

	_gg_iniGraf();			// inicializar gráficos
	
	if (!nitroFSInit()) {
		_gg_escribir("ERROR: ¡no se puede utilizar NitroFS!", 0);
		exit(0);
	}

	irqInitHandler(_gp_IntrMain);	// inicializar IRQs
	irqSet(IRQ_VBLANK, _gp_rsiVBL);
	irqEnable(IRQ_VBLANK);
	
	irqSet(IRQ_TIMER0, _gs_rsiTIMER0);
	irqEnable(IRQ_TIMER0);				// instalar la RSI para el TIMER0
	TIMER0_DATA = divFreq0; 
	TIMER0_CR = 0xC3;  		// Timer Start | Timer IRQ Enabled | Prescaler Selection 3 (F/1024)
}


//------------------------------------------------------------------------------
int main(int argc, char **argv) {
//------------------------------------------------------------------------------
	intFunc start;

	inicializarSistema();
	
	_gg_escribir("********************************", 0);
	_gg_escribir("*                              *", 0);
	_gg_escribir("* Sistema Operativo GARLIC 1.0 *", 0);
	_gg_escribir("*                              *", 0);
	_gg_escribir("********************************", 0);
	_gg_escribir("*** Inicio fase 2 / ProgP\n", 0);

	_gg_escribir("*** Carga de programa HOLA.elf\n", 0);
	start = _gm_cargarPrograma("HOLA");
	if (start)
	{
		_gp_crearProc(start, 1, "HOLA", 2);
		_gp_crearProc(start, 2, "HOLA", 3);
		_gp_crearProc(start, 3, "HOLA", 3);
		
		while (_gd_tickCount < 300)			// esperar 5 segundos
		{
			_gp_WaitForVBlank();
			porcentajeUso();
		}
		_gp_matarProc(1);					// matar proceso 1
		_gg_escribir("Proceso 1 eliminado\n", 0);
		
		while (_gd_tickCount < 600)			// esperar 5 segundos más
		{
			_gp_WaitForVBlank();
			porcentajeUso();
		}
		_gp_matarProc(3);					// matar proceso 3
		_gg_escribir("Proceso 3 eliminado\n", 0);
		
		while (_gp_numProc() > 1)			// esperar a que proceso 2 acabe
		{
			_gp_WaitForVBlank();
			porcentajeUso();
		}
		_gg_escribir("Proceso 2 terminado\n", 0);
		
	} else
		_gg_escribir("*** Programa NO cargado\n", 0);

	_gg_escribir("*** Carga de programa PONG.elf\n", 0);
	start = _gm_cargarPrograma("PONG");
	if (start)
	{
		_gp_crearProc(start, 1, "PONG", 1);
		_gp_crearProc(start, 2, "PONG", 2);
		_gp_crearProc(start, 3, "PONG", 3);
		
		while (_gd_tickCount < 3600)			// esperar hasta 60 segundos
		{
			_gp_WaitForVBlank();
			porcentajeUso();
		}
		
		_gp_matarProc(1);					// matar los 3 procesos a la vez
		_gp_matarProc(2);
		_gp_matarProc(3);
		_gg_escribir("Procesos 1, 2 y 3 eliminados\n", 0);
		
		while (_gp_numProc() > 1)	// esperar a que todos los procesos acaben
		{
			_gp_WaitForVBlank();
			porcentajeUso();
		}
		
	} else
		_gg_escribir("*** Programa NO cargado\n", 0);

	_gg_escribir("*** Final fase 2 / ProgP\n", 0);
	
	while(1) {
		_gp_WaitForVBlank();
	}							// parar el procesador en un bucle infinito
	return 0;
}


