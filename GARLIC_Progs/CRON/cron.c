/*------------------------------------------------------------------------------

	"CRON.c" : cuarto programa de prueba para el sistema operativo GARLIC;
	
	Muestra un cron�metro por la ventana del proceso, con minutos y segundos,
	utilizando matrices de 8x8 caracteres para visualizar los d�gitos y el
	car�cter ':', mediante la funcion GARLIC_printmat.
	El argumento del programa indicar� el tiempo a esperar entre incrementos
	de segundo, adem�s de determinar el color de los d�gitos;
		0:	desbancar el proceso (blanco)
		1:	un segundo, es decir, tiempo real (amarillo)
		2:	dos segundos (verde)
		3:	tres segundos (rojo)

------------------------------------------------------------------------------*/

#include <GARLIC_API.h>			/* definici�n de las funciones API de GARLIC */

const char digitos[][8][8] =
	{	// d�gito 0
	 {{	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x7F, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x7F, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}},
		// d�gito 1
	 {{	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}},
		// d�gito 2
	 {{	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x20, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}},
		// d�gito 3
	 {{	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}},
		// d�gito 4
	 {{	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}},
		// d�gito 5
	 {{	0x20, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x7F, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}},
		// d�gito 6
	 {{	0x20, 0x20, 0x20, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x20, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}},
		// d�gito 7
	 {{	0x20, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}},
		// d�gito 8
	 {{	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}},
		// d�gito 9
	 {{	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x20, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}},
		// car�cter ':'
	 {{	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x20, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x20, 0x20, 0x20, 0x20, 0},
	  {	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}}};


int _start(int arg)				/* funci�n de inicio : no se usa 'main' */
{
	int color;
	int umin, dmin, useg, dseg;
	unsigned char masc;
	
	GARLIC_clear();
	GARLIC_print("-- Programa CRON (Garlic 1.0) --", 2);

	color = arg & 3;				// asignar un color en funci�n del argumento
	dmin = 0; umin = 0;
	dseg = 0; useg = 0;	
									// imprimir "00:00"
	GARLIC_printmat(0, 8, (char (*)[8]) digitos[0], color);
	GARLIC_printmat(7, 8, (char (*)[8]) digitos[0], color);
	GARLIC_printmat(14, 8, (char (*)[8]) digitos[10], color);
	GARLIC_printmat(19, 8, (char (*)[8]) digitos[0], color);
	GARLIC_printmat(26, 8, (char (*)[8]) digitos[0], color);
	do
	{
		GARLIC_delay(color);		// retardo dependiendo del argumento
		useg++;						// incrementar unidades de segundo
		masc = 0;					// resetear m�scara de modificaciones
		if (useg == 10)
		{	useg = 0;				// desbordamiento de unidades de segundo
			dseg++;
			masc |= 0x01;	
			if (dseg == 6)
			{	dseg = 0;			// desbordamiento de decenas de segundo
				umin++;
				masc |= 0x02;
				if (umin == 10)
				{	umin = 0;		// desbordamiento de unidades de minuto
					dmin++;
					masc |= 0x04;
				}
			}
		}
		GARLIC_printmat(26, 8, (char (*)[8]) digitos[useg], color);
		if (masc & 0x01)
			GARLIC_printmat(19, 8, (char (*)[8]) digitos[dseg], color);
		if (masc & 0x02)
			GARLIC_printmat(7, 8, (char (*)[8]) digitos[umin], color);
		if (masc & 0x04)
			GARLIC_printmat(0, 8, (char (*)[8]) digitos[dmin], color);	
	} while (dmin < 9); 			// acaba a los 90 minutos
	return 0;
}
