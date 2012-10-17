/*------------------------------------------------------------------------------

	"main.c" : fase 1 / programador G

	Programa de prueba para la gesti�n del sistema gr�fico de GARLIC, donde
	cada proceso dispone de una zona de 24 l�neas por 32 caracteres para
	escribir mensajes.

------------------------------------------------------------------------------*/
#include <nds.h>

#include <garlic_system.h>	// definici�n de funciones y variables de sistema

#include <GARLIC_API.h>		// inclusi�n del API para simular un proceso
int hola();					// funci�n que simula la ejecuci�n del proceso

extern int * punixTime;		// puntero a zona de memoria con el tiempo real


/* Inicializaciones generales del sistema Garlic */
//------------------------------------------------------------------------------
void inicializarSistema() {
//------------------------------------------------------------------------------
	
	_gg_iniGraf();		// inicializar gr�ficos
	
	_gd_seed = *punixTime;	// inicializar semilla para n�meros aleatorios con
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
	
	_gd_pidz = 3;	// simular z�calo 3
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
							// c�lculo del n�mero aleatorio de iteraciones
	GARLIC_divmod((unsigned) GARLIC_random(), 100, &i, &iter);
	iter++;
	//iter = 	23;				// asegurar que hay al menos una iteraci�n
	for (i = 0; i < iter; i++)
	{
		GARLIC_num2str(num, 3, i);
		GARLIC_print(num);
		GARLIC_print("\tHello world!\n");
	}
	return 0;
}

