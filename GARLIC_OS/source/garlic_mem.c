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
	
	const char* path = "/Programas/";
	//const char* filename = "hola.elf");
	const char* filename = ("awesum.txt");
	const char* file_n = ("/Programas/hola.elf");
	//const char* file_n = ("/Programas/awesum.txt");
	
	
	
	FILE* inf = fopen(file_n,"rb");
	
	if(inf)
	{
		printf("\nyes\n");
		
		int len;
		
		fseek(inf,0,SEEK_END);
		len = ftell(inf);
		fseek(inf,0,SEEK_SET);
		
		iprintf("\nthe following %d bytes message\nfrom file1.txt is\nbrought to you by fread:\n",len);
		{
			char *entireFile = (char*)malloc(len+1);
			entireFile[len] = 0;
			if(fread(entireFile,1,len,inf) != len)
				iprintf("savage error reading the bytes from the file!\n");
			else
				iprintf("%s\n-done-\n",entireFile);
			free(entireFile);
		}
			
		fclose(inf);
		system("arm-eabi-readelf -h hola.elf");
	}
		
	else printf("\nno\n");
	
	

	return ((intFunc) result);
}



