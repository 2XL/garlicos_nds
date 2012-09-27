/*------------------------------------------------------------------------------

	"garlic_mem.c" : fase 1 / programador M

	Rutinas de carga de un fichero ejecutable en formato ELF, para el sistema
	GARLIC 0.5

------------------------------------------------------------------------------*/
#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include <string.h>

#include <garlic_system.h>	// definici�n de funciones y variables de sistema



/* _gm_relocatar: a partir de una direcci�n de memoria (destino), de un vector
			que contiene todo el fichero ELF (file) y de una direcci�n 'virtual'
			de inicio de un segmento (pAddr), esta funci�n interpretar� los
			'relocs' del fichero ELF y ajustar� las direcciones de memoria
			correspondientes a relocs de tipo R_ARM_ABS32, restando la direcci�n
			virtual y sumando la direcci�n destino */
void _gm_relocatar(unsigned int *destino, char *file, unsigned int pAddr)
{

}

/* _gm_cargarPrograma: cargar el programa "(keyName).elf" en una zona de memoria
			libre y relocatar las referencias a los s�mbolos del programa; */
//---------------------------------------------------------------------------------
intFunc _gm_cargarPrograma(char *keyName) {
//---------------------------------------------------------------------------------
	unsigned int result = 0;

	return ((intFunc) result);
}



