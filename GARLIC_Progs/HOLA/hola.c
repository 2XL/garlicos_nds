/*------------------------------------------------------------------------------

	"HOLA.c" : primer programa de prueba para el sistema operativo GARLIC;
	
	Imprime el típico mensaje "Hello world!" por una ventana de GARLIC, un
	número de veces igual a 10 elevado al argumento, el cual se limita al
	rango [0..3].

------------------------------------------------------------------------------*/

#include <GARLIC_API.h>			/* definición de las funciones API de GARLIC */

int _start(int arg)				/* función de inicio : no se usa 'main' */
{
	unsigned int i, iter;
	char num[5];
	
	GARLIC_print("-- Programa HOLA (Garlic 1.0) --", 2);
	iter = 1;
	if (arg > 3) arg = 3;	// limitar valor del argumento
	for (i = 0; i < arg; i++)
		iter *= 10;			// iteraciones = 10 elevado a valor del argumento
	for (i = 0; i < iter; i++)
	{
		GARLIC_num2str(num, 5, i);
		GARLIC_print(num, 1);
		GARLIC_print("\tHello world!\n", 0);
	}
	return 0;
}
