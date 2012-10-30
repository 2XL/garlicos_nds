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



#define MEMORIA_PROGRAMA 0x02100000



// Actualiza la posición y devuelve el word
int juntar_word (char *file, int* pos)
{
	int num = *pos;
	*pos = *pos + 4;
	return (file[num+3] << 24) | (file[num+2] << 16) | (file[num+1] << 8) | file[num];
}


// Actualiza la posición y devuelve el half-word
int juntar_half (char *file, int* pos)
{
	int num = *pos;
	*pos = *pos + 2;
	return (file[num+1] << 8) | file[num];
}






/* _gm_relocatar: a partir de una dirección de memoria (destino), de un vector
			que contiene todo el fichero ELF (file) y de una dirección 'virtual'
			de inicio de un segmento (pAddr), esta función interpretará los
			'relocs' del fichero ELF y ajustará las direcciones de memoria
			correspondientes a relocs de tipo R_ARM_ABS32, restando la dirección
			virtual y sumando la dirección destino */
void _gm_relocatar(unsigned int *destino, char *file, unsigned int pAddr)
{
	
	int pos=24;									//pos del entry
	
	pos=pos+8;	//Saltamos entry y phoff		//pos 24 -> 32
	int shoff =		juntar_word (file, &pos);	//pos 32 -> 36
	pos=pos+10;	//Saltamos phentsize y phnum	//pos 36 -> 46
	int shentsize =	juntar_half (file, &pos);	//pos 46 -> 48
	int shnum = 	juntar_half (file, &pos);	//pos 48 -> 50
	
	
	// Variables de las secciones
	pos = shoff;
	int pos_ini = pos;
	int contador, sh_offset, sh_size, sh_link, sh_info, sh_entsize;
	
	// Variables de relocalizaciones
	int r_offset, contador_r, pos_r, pos_r_ini;
	unsigned int* direccion;					// Para trabajar con la dirección destino sin cambiar su variable
	
	
	
	for (contador=0; contador<shnum; contador++, pos=pos_ini+shentsize, pos_ini=pos)	// Mientras haya más secciones...
	{
		pos=pos+4;		// Nos saltamos sh_name
		// Sólo nos interesan los que tengan type 9
		if ( juntar_word (file, &pos) == 9)	// si sh_type es 9 (SHT REL)  pos  shoff -> shoff+4
		{
			pos=pos+8;	// Saltamos sh_flags y sh_addr    pos  shoff+4  -> shoff+12
			sh_offset = juntar_word (file, &pos);		//pos  shoff+12 -> shoff+16
			sh_size = 	juntar_word (file, &pos);		//pos  shoff+16 -> shoff+20
			sh_link = 	juntar_word (file, &pos);		//pos  shoff+20 -> shoff+24
			sh_info = 	juntar_word (file, &pos);		//pos  shoff+24 -> shoff+28
			pos=pos+4;	// Nos saltamos sh_addralign  	  pos  shoff+28 -> shoff+32
			sh_entsize =juntar_word (file, &pos);		//pos  shoff+32 -> shoff+36
			
			
			
			// Relocalizaciones
			pos_r = sh_offset;
			pos_r_ini = pos_r;
			printf(">> seccion relocalizadores ( ):\n");
			
			for (contador_r=0; contador_r<(sh_size/sh_entsize); contador_r++, pos_r=pos_r_ini+sh_entsize, pos_r_ini=pos_r)
			{
				// if r_info == 514 (0x0202)  - r_info tiene un offset inicial de 4 bytes respecto a r_offset, de ahí que sume para acceder al info y luego reste para el offset
				pos_r = pos_r+4;
				if ( juntar_word (file, &pos_r) == 514)		//pos_r  sh_offset+4 -> sh_offset+8
				{
					pos_r = pos_r-8;							//pos_r  sh_offset+8 -> sh_offset
					r_offset =	juntar_word (file, &pos_r);		//pos_r  sh_offset   -> sh_offset+4
					
					printf(">>> rel %d (R_ARM_ABS32): 0x%X\n", contador_r, r_offset);
					
					direccion = (unsigned int*)((int)destino + r_offset - pAddr);
					*(direccion) = (int)destino + *(direccion) - pAddr ;
					
				}	//endif r
				
			}	// endfor r
			
		}	// endif sh
		
	}	// endfor sh
	
	
}





/* _gm_cargarPrograma: cargar el programa "(keyName).elf" en una zona de memoria
			libre y relocatar las referencias a los símbolos del programa; */
//---------------------------------------------------------------------------------
intFunc _gm_cargarPrograma(char *keyName) {
//---------------------------------------------------------------------------------
	
	unsigned int result = 0;
	
	const char* path = "/Programas/";
	const char* extension = ".elf";
	
	// NOTA -- keyName es en mayus, así que hay que cambiar el nombre del archivo generado hola.elf por HOLA.elf
	
	char filename[19];
	strcpy(filename,path);
	strcat(filename,keyName);
	strcat(filename,extension);
	
	FILE* inf = fopen(filename,"rb");
	
	
	if(inf)
	{
		int len;
		
		fseek(inf,0,SEEK_END);
		len = ftell(inf);
		fseek(inf,0,SEEK_SET);
		
		
		char* puntero = (char*)malloc(len);
		if (fread(puntero, 1, len, inf) != len)
			iprintf("Error al leer bytes del archivo\n");
		else
		{			
			
			//int entry, phoff, shoff, phentsize, phnum, shentsize, shnum;
			int entry, phoff, phentsize, phnum;
			
			/*  EI_NIDENT 16 nos dice que e_ident estará en la decimosexta posición, puntero[15]
				Por lo tanto, si vamos sumando el num de bytes de cada variable de la estructura:
				pos[15] e_ident			pos[16-17] e_type		pos[18-19] e_machine
				pos[20-23] e_version	pos[24-27] e_entry		pos[28-31] e_phoff
				pos[32-35] e_shoff		pos[36-39] e_flags		pos[40-41] e_ehsize
				pos[42-43] e_phentsize	pos[44-45] e_phnum		pos[46-47] e_shentsize
				pos[48-49] e_shnum		pos[50-51] e_shstrnx
				
				Como los que necesitamos usan más de un byte habrá que unirlos con la función juntar_"tipo"
			*/
			
			int pos=24;									//pos del entry
			
			entry = 	juntar_word (puntero, &pos);	//pos 24 -> 28
			phoff =		juntar_word (puntero, &pos);	//pos 28 -> 32
			pos=pos+10;									//pos 32 -> 42
			phentsize =	juntar_half (puntero, &pos);	//pos 42 -> 44
			phnum = 	juntar_half (puntero, &pos);	//pos 44 -> 46
			
			
			printf("> cabeceras de programa :   %d\n", phnum);
			
			
			// Variables segmentos
			int destino = MEMORIA_PROGRAMA;		// 0x02100000
			pos = phoff;
			int pos_ini = pos;
			int p_paddr = 0;					// Para evitar el warning de que podría no estar declarada
			int contador, p_offset, p_filesz, p_memsz, p_flags;
			
			
			
			for (contador=0; contador<phnum; contador++, pos=pos_ini+phentsize, pos_ini=pos)	// Mientras haya más segmentos...
			{
				// Sólo nos interesan los que tengan type 1
				if (juntar_word (puntero, &pos) == 1) // Si el p_type es 1 (PT_LOAD)    	pos phoff -> phoff+4
				{
					p_offset = 	juntar_word (puntero, &pos);	//pos phoff+4  -> phoff+8
					pos=pos+4;	// nos saltamos p_vaddr			  pos phoff+8  -> phoff+12
					p_paddr = 	juntar_word (puntero, &pos);	//pos phoff+12 -> phoff+16
					p_filesz = 	juntar_word (puntero, &pos);	//pos phoff+16 -> phoff+20
					p_memsz = 	juntar_word (puntero, &pos);	//pos phoff+20 -> phoff+24
					p_flags = 	juntar_word (puntero, &pos);	//pos phoff+24 -> phoff+28
					
					
					// Copiar a la memoria
					dmaCopy((const void*)&puntero[p_offset], (void*)destino, p_filesz);
					
					
					printf("> segmento num. %d copiado:\n", contador);
					printf("   despl. en fichero :  %d\n", p_offset);
					printf("   tam. en fichero : %d\n", p_filesz);
					printf("   tam. en memoria : %d\n", p_memsz);
					
					
					_gm_relocatar((unsigned int*)destino, puntero, p_paddr);
					
					destino = destino + p_memsz;				// Se actualiza destino
					
					
					
				}	// endif p
				
			}	// endfor p
			
			
			free(puntero);
			result = MEMORIA_PROGRAMA + entry - p_paddr;				// La función debe devolver esta dirección de arranque
			
		}
		
		fclose(inf);
		
	}
	

	return ((intFunc) result);
}



