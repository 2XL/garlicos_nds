/*------------------------------------------------------------------------------

	"BORR.c" : quinto programa de prueba para el sistema operativo GARLIC;
	
	Borra el contenido de la ventana y termina.

------------------------------------------------------------------------------*/

#include <GARLIC_API.h>			/* definici�n de las funciones API de GARLIC */

int _start(int arg)				/* funci�n de inicio : no se usa 'main' */
{
	GARLIC_clear();
	return 0;
}
