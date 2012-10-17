/*------------------------------------------------------------------------------

	"garlic_graf.c" : fase 1 / programador G

	Rutinas de gesti�n del entorno gr�fico (ventanas de texto), para el sistema
	GARLIC 0.5

------------------------------------------------------------------------------*/
#include <nds.h>

#include <garlic_system.h>	// definici�n de funciones y variables de sistema
#include <garlic_font.h>	// definici�n gr�fica de caracteres


int bg2A_id, bg3A_id;
int scale = 1<<8; // que en sistema de coma fija es 256 para el caso de 8 en factor de escala.
 
char marcoFondo3[48][128];
char marcoFondo2[48][128];
 
//FUNCIONS AUXILIARS...

void _gg_zocaloScroll ();
void _gg_initMarco();

/* Inicializaciones gr�ficas del sistema Garlic */
//------------------------------------------------------------------------------
void _gg_iniGraf()
//------------------------------------------------------------------------------
{

}


/* Escribir un mensaje de texto, como vector de caracteres terminado por cero,
	en la ventana correspondiente al z�calo indicado por par�metro */
//------------------------------------------------------------------------------
void _gg_escribir(char *mensaje, int zocalo)
//------------------------------------------------------------------------------
{

}

