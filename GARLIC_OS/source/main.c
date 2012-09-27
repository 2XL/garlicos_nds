/*------------------------------------------------------------------------------

	"main.c" : fase 1 / programador P

	Programa de prueba para la creación de un proceso similar al programa HOLA,
	que se simulará con una función y se ejecutará concurrentemente con el
	código del sistema operativo.

------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>

#include <garlic_system.h>	// definición de funciones y variables de sistema

#include <GARLIC_API.h>		// inclusión del API para simular un proceso
int hola();					// función que simula la ejecución del proceso

extern int * punixTime;		// puntero a zona de memoria con el tiempo real


/* Inicializaciones generales del sistema Garlic */
//------------------------------------------------------------------------------
void inicializarSistema() {
//------------------------------------------------------------------------------
	
	consoleDemoInit();		// inicializar consola libNDS para realizar pruebas
	
	_gd_seed = *punixTime;	// inicializar semilla para números aleatorios con
	_gd_seed <<= 16;		// el valor de tiempo real UNIX, desplazado 16 bits
	
	irqInitHandler(_gp_IntrMain);	// inicializar IRQs
	irqSet(IRQ_VBLANK, _gp_rsiVBL);
	irqEnable(IRQ_VBLANK);
	
	_gd_psv[0].keyName = 0x4C524147;	// "GARL"
}


//------------------------------------------------------------------------------
int main(int argc, char **argv) {
//------------------------------------------------------------------------------
	
	inicializarSistema();
	
	_gp_WaitForVBlank();
	printf("********************************");
	printf("*                              *");
	printf("* Sistema Operativo GARLIC 0.5 *");
	printf("*                              *");
	printf("********************************");
	printf("*** Inicio fase 1_P\n");
	
	_gp_crearProc(hola, 7, "HOLA");

	while(_gp_numProc() > 1) {
		_gp_WaitForVBlank();
		printf("*** Test\t%d\t%d\n", _gd_tickCount, _gp_numProc());
	}							// esperar a que termine el proceso pendiente

	printf("*** Final fase 1_P\n");

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
	GARLIC_divmod((unsigned) GARLIC_random(), 10, &i, &iter);
	iter++;					// asegurar que hay al menos una iteración
	for (i = 0; i < iter; i++)
	{
		GARLIC_num2str(num, 3, i);
		GARLIC_print(num);
		GARLIC_print(": Hello world!\n");
	}
	return 0;
}


/* rutina de soporte para _gi_print */
//------------------------------------------------------------------------------
void _gg_escribir(char * message, int z) {
//------------------------------------------------------------------------------
	int i, j;
	char c, n;
	
	j = _gd_psv[z].pControl;		// obtener índice actual de caracteres
	i = 0;							// 'i' es índice de message
	n = 0;							// 'n' indica si ha encontrado un '\n'
	do
	{	c = message[i++];	
		_gd_psv[z].pChars[j++] = c;	// copia de caracteres en pChars
		if (c == '\n') n = 1;			// detección de '\n'
	} while (c != 0);					// hasta final de string

	if ((n == 1) || (j >= 32))			// si ha detectado un '\n' o final de linea
	{	_gp_WaitForVBlank();
		printf("%s", _gd_psv[z].pChars);	// imprimir toda la línea
		j = 0;								// y reiniciar contador
	}
	else j--;						// sino, ignorar último carácter (fin string)
	_gd_psv[z].pControl = j;		// actualizar contador de caracteres
}

