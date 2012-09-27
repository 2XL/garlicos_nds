/*------------------------------------------------------------------------------

	"garlic_mem.c" : fase 1 / programador M

	Rutinas de carga de un fichero ejecutable en formato ELF, para el sistema
	GARLIC 0.5

------------------------------------------------------------------------------*/
#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include <string.h>

#include <garlic_system.h>	// definición de funciones y variables de sistema



/* _gm_relocatar: a partir de una dirección de memoria (destino), de un vector
			que contiene todo el fichero ELF (file) y de una dirección 'virtual'
			de inicio de un segmento (pAddr), esta función interpretará los
			'relocs' del fichero ELF y ajustará las direcciones de memoria
			correspondientes a relocs de tipo R_ARM_ABS32, restando la dirección
			virtual y sumando la dirección destino */
void _gm_relocatar(unsigned int *destino, char *file, unsigned int pAddr)
{

}

/* _gm_cargarPrograma: cargar el programa "(keyName).elf" en una zona de memoria
			libre y relocatar las referencias a los símbolos del programa; */
//---------------------------------------------------------------------------------
intFunc _gm_cargarPrograma(char *keyName) {
//---------------------------------------------------------------------------------
	unsigned int result = 0;

	return ((intFunc) result);
}



