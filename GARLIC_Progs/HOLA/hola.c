/*------------------------------------------------------------------------------

	"HOLA.c" : primer programa de prueba para el sistema operativo GARLIC;
	
	Imprime el típico mensaje "Hello world!" por una ventana de GARLIC, un
	número aleatorio de veces.

------------------------------------------------------------------------------*/

#include <GARLIC_API.h>			/* definición de las funciones API de GARLIC */

int _start(void)					/* función de inicio : no se usa 'main' */
{
	unsigned int i, iter;
	char num[5];
	
	GARLIC_print("-- Programa HOLA (Garlic 0.5) --");
							// cálculo del número aleatorio de iteraciones
	GARLIC_divmod((unsigned) GARLIC_random(), 100, &i, &iter);
	iter++;					// asegurar que hay al menos una iteración
	for (i = 0; i < iter; i++)
	{
		GARLIC_num2str(num, 5, i);
		GARLIC_print(num);
		GARLIC_print("\tHello world!\n");
	}
	return 0;
}
