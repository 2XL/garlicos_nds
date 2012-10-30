/*------------------------------------------------------------------------------

	"BORR.c" : quinto programa de prueba para el sistema operativo GARLIC;
	
	Borra el contenido de la ventana y termina.

------------------------------------------------------------------------------*/

#include <GARLIC_API.h>			/* definición de las funciones API de GARLIC */

int _start(int arg)				/* función de inicio : no se usa 'main' */
{
	GARLIC_clear();
	return 0;
}
