/*------------------------------------------------------------------------------

	"HOLA.c" : primer programa de prueba para el sistema operativo GARLIC;
	
	Imprime el t�pico mensaje "Hello world!" por una ventana de GARLIC, un
	n�mero aleatorio de veces.

------------------------------------------------------------------------------*/

#include <GARLIC_API.h>			/* definici�n de las funciones API de GARLIC */

int _start(void)					/* funci�n de inicio : no se usa 'main' */
{
	unsigned int i, iter;
	char num[5];
	
	GARLIC_print("-- Programa HOLA (Garlic 0.5) --");
							// c�lculo del n�mero aleatorio de iteraciones
	GARLIC_divmod((unsigned) GARLIC_random(), 100, &i, &iter);
	iter++;					// asegurar que hay al menos una iteraci�n
	for (i = 0; i < iter; i++)
	{
		GARLIC_num2str(num, 5, i);
		GARLIC_print(num);
		GARLIC_print("\tHello world!\n");
	}
	return 0;
}
