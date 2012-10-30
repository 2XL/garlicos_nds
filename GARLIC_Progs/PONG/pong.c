/*------------------------------------------------------------------------------

	"PONG.c" : tercer programa de prueba para el sistema operativo GARLIC;
	
	Rebota un caracter por la ventana del proceso, donde la velocidad de
	movimiento del caracter depende del argumento del programa, que corresponde
	al n�mero de segundos que tiene que esperar entre movimiento y movimiento;
	si el argumento es 0, el retardo es un retrazado vertical (m�s el tiempo
	necesario para restaurar el proceso).

------------------------------------------------------------------------------*/

#include <GARLIC_API.h>			/* definici�n de las funciones API de GARLIC */


int _start(int arg)				/* funci�n de inicio : no se usa 'main' */
{
	int color;
	int x, y, dirx, diry;
	
	GARLIC_clear();
	GARLIC_print("-- Programa PONG (Garlic 1.0) --", 2);

	if (arg > 6) arg = 6;		// limitar retardo m�ximo 6 segundos
	color = arg & 3;				// asignar un color en funci�n del argumento
	x = 0; y = 0;					// posici�n inicial
	dirx = 1; diry = 1;				// direcci�n inicial
	GARLIC_printchar( x, y, 95, color);	// escribir caracter por primera vez
	do
	{
		GARLIC_delay(arg);
		GARLIC_printchar( x, y, 0, color);	// borrar caracter anterior
		x += dirx;
		y += diry;						// avance del caracter
		if ((x == 31) || (x == 0))
			dirx = -dirx;				// rebote izquierda o derecha
		if ((y == 23) || (y == 0))
			diry = -diry;				// rebote arriba o abajo
		if ((x == 0) && (y == 0))
			y = 1;						// forzar posiciones (x+y) impares
		else if ((x == 0) && (y == 1))
		{	y = 0;						// forzar posiciones (x+y) pares
			diry = 1;						// forzar direcci�n derecha
		}
		GARLIC_printchar( x, y, 95, color);	// reescribir caracter		
	} while (1); 				// no acaba nunca
	return 0;
}
