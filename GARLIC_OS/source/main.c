/*------------------------------------------------------------------------------

	"main.c" : fase 1 / programador G

	Programa de prueba para la gestión del sistema gráfico de GARLIC, donde
	cada proceso dispone de una zona de 24 líneas por 32 caracteres para
	escribir mensajes.

------------------------------------------------------------------------------*/
#include <nds.h>

#include <garlic_system.h>	// definición de funciones y variables de sistema

#include <GARLIC_API.h>		// inclusión del API para simular un proceso
int hola();					// función que simula la ejecución del proceso

extern int * punixTime;		// puntero a zona de memoria con el tiempo real


/* Inicializaciones generales del sistema Garlic */
//------------------------------------------------------------------------------
void inicializarSistema() {
//------------------------------------------------------------------------------
	
	_gg_iniGraf();		// inicializar gráficos
	
	_gd_seed = *punixTime;	// inicializar semilla para números aleatorios con
	_gd_seed <<= 16;		// el valor de tiempo real UNIX, desplazado 16 bits
	
	irqInitHandler(_gp_IntrMain);	// inicializar IRQs
	irqSet(IRQ_VBLANK, _gp_rsiVBL);
	irqEnable(IRQ_VBLANK);
}


//------------------------------------------------------------------------------
int main(int argc, char **argv) {
//------------------------------------------------------------------------------

	inicializarSistema();
	
	_gg_escribir("********************************", 0);
	_gg_escribir("*                              *", 0);
	_gg_escribir("* Sistema Operativo GARLIC 0.5 *", 0);
	_gg_escribir("*                              *", 0);
	_gg_escribir("********************************", 0);
	_gg_escribir("*** Inicio fase 1_G\n", 0);
	
	_gd_pidz = 3;	// simular zócalo 3
	hola();

	_gg_escribir("*** Final fase 1_G\n", 0);

	while(1) {
		_gp_WaitForVBlank();
	}							// parar el procesador en un bucle infinito
	return 0;
}


/* Proceso de prueba */
//------------------------------------------------------------------------------
int hola() {
//------------------------------------------------------------------------------
	unsigned int i, iter;
	char num[3];
	
	GARLIC_print("-- Programa HOLA (Garlic 0.5) --");
							// cálculo del número aleatorio de iteraciones
	GARLIC_divmod((unsigned) GARLIC_random(), 100, &i, &iter);
	iter++;					// asegurar que hay al menos una iteración
	for (i = 0; i < iter; i++)
	{
		GARLIC_num2str(num, 3, i);
		GARLIC_print(num);
		GARLIC_print("\tHello world!\n");
	}
	return 0;
}

