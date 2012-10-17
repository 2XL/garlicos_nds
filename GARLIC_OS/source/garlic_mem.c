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
	
	//file[(int)destino] = (int)0x02100000 + file[(int)destino] - pAddr ;
	*(destino) = (int)0x02100000 + *(destino) - pAddr ;
	
}

/* _gm_cargarPrograma: cargar el programa "(keyName).elf" en una zona de memoria
			libre y relocatar las referencias a los s�mbolos del programa; */
//---------------------------------------------------------------------------------
intFunc _gm_cargarPrograma(char *keyName) {
//---------------------------------------------------------------------------------
	
	unsigned int result = 0;
	
	const char* path = "/Programas/";
	const char* extension = ".elf";
	
	// NOTA -- keyName es en mayus, as� que hay que cambiar el nombre del archivo generado hola.elf por HOLA.elf
	
	char filename[100];
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
		
		
		char* puntero = (char*)malloc(len+1);
		puntero[len] = 0;
		if (fread(puntero, 1, len, inf) != len)
			iprintf("Error al leer bytes del archivo\n");
		else
		{			
			
			int entry, phoff, shoff, phentsize, phnum, shentsize, shnum;
			
			/*  EI_NIDENT 16 nos dice que e_ident estar� en la decimosexta posici�n, puntero[15]
				Por lo tanto, si vamos sumando el num de bytes de cada variable de la estructura:
				pos[15] e_ident			pos[16-17] e_type		pos[18-19] e_machine
				pos[20-23] e_version	pos[24-27] e_entry		pos[28-31] e_phoff
				pos[32-35] e_shoff		pos[36-39] e_flags		pos[40-41] e_ehsize
				pos[42-43] e_phentsize	pos[44-45] e_phnum		pos[46-47] e_shentsize
				pos[48-49] e_shnum		pos[50-51] e_shstrnx
				
				Como todos los que necesitamos usan m�s de un byte habr� que concatenarlos.
			*/
			
			entry = 	(puntero[27] << 24) | (puntero[26] << 16) | (puntero[25] << 8) | puntero[24];
			phoff =		(puntero[31] << 24) | (puntero[30] << 16) | (puntero[29] << 8) | puntero[28];
			shoff =		(puntero[35] << 24) | (puntero[34] << 16) | (puntero[33] << 8) | puntero[32];
			phentsize =	(puntero[43] << 8) | puntero[42];
			phnum = 	(puntero[45] << 8) | puntero[44];
			shentsize =	(puntero[47] << 8) | puntero[46];
			shnum = 	(puntero[49] << 8) | puntero[48];
			
			//printf("%d %d %d %d %d %d %d\n", entry, phoff, shoff, phentsize, phnum, shentsize, shnum);
			
			printf("> cabeceras de programa :   %d\n", phnum);
			
			
			
			// Declaraci�n variables de secciones
			int contador_sh, pos_sh, pos_sh_ini;
			int sh_offset, sh_size, sh_link, sh_info, sh_entsize;
			
			//Declaraci�n variables de relocalizaciones
			int contador_r, pos_r, pos_r_ini;
			int r_offset;
			unsigned int* destino;
			
			// Variables segmentos
			int contador;
			int mem_offset = 0;					// Para determinar d�nde cargar segmentos si hay m�s de 1
			int pos = phoff;
			int pos_ini = pos;
			int p_offset, p_paddr, p_filesz, p_memsz, p_flags;
			
			
			
			for (contador=0; contador<phnum; contador++, pos=pos_ini+phentsize, pos_ini=pos)	// Mientras haya m�s segmentos...
			{
				// S�lo nos interesan los que tengan type 1
				if (( (puntero[pos+3] << 24) | (puntero[pos+2] << 16) | (puntero[pos+1] << 8) | puntero[pos] ) == 1) // Si el p_type es 1 (PT_LOAD)
				{
					pos=pos+4;
					p_offset = 	(puntero[pos+3] << 24) | (puntero[pos+2] << 16) | (puntero[pos+1] << 8) | puntero[pos];
					pos=pos+8;	// nos saltamos p_vaddr
					p_paddr = 	(puntero[pos+3] << 24) | (puntero[pos+2] << 16) | (puntero[pos+1] << 8) | puntero[pos];
					pos=pos+4;
					p_filesz = 	(puntero[pos+3] << 24) | (puntero[pos+2] << 16) | (puntero[pos+1] << 8) | puntero[pos];
					pos=pos+4;
					p_memsz = 	(puntero[pos+3] << 24) | (puntero[pos+2] << 16) | (puntero[pos+1] << 8) | puntero[pos];
					pos=pos+4;
					p_flags = 	(puntero[pos+3] << 24) | (puntero[pos+2] << 16) | (puntero[pos+1] << 8) | puntero[pos];
					//pos=pos+4;	// nos saltamos p_align
					
					//printf("%d %d %d %d %d\n", p_offset, p_paddr, p_filesz, p_memsz, p_flags);
					
					
					
					// CARGAR EL CONTENIDO DEL SEGMENTO EN DIRECCI�N DESTINO
					// 
					//dmaCopy((void*)0x02100000, (const void*)puntero[entry], (24*1024));
					//
					//DC_FlushRange((const void*)p_offset, p_memsz);
					
					
					//dmaCopy((const void*)p_offset, (void*)0x02100000 + mem_offset, p_memsz);
					dmaCopy((const void*)&puntero[p_offset], 0x02100000 + mem_offset, p_memsz);
					mem_offset = mem_offset + p_memsz;
					//
					//
					
					
					
					printf("> segmento num. %d copiado:\n", contador);
					printf("   despl. en fichero :  %d\n", p_offset);
					printf("   tam. en fichero : %d\n", p_filesz);
					printf("   tam. en memoria : %d\n", p_memsz);
					
					
					pos_sh = shoff;										// Trabajaremos con las secciones
					pos_sh_ini = pos_sh;
					
					for (contador_sh=0; contador_sh<shnum; contador_sh++, pos_sh=pos_sh_ini+shentsize, pos_sh_ini=pos_sh)	// Mientras haya m�s secciones...
					{
						pos_sh=pos_sh+4;	// Nos saltamos sh_name
						// S�lo nos interesan los que tengan type 9
						if (( (puntero[pos_sh+3] << 24) | (puntero[pos_sh+2] << 16) | (puntero[pos_sh+1] << 8) | puntero[pos_sh] ) == 9)	// si sh_type es 9 (SHT REL)
						{
							pos_sh=pos_sh+12;	// Nos saltamos sh_flags y sh_addr
							sh_offset = (puntero[pos_sh+3] << 24) | (puntero[pos_sh+2] << 16) | (puntero[pos_sh+1] << 8) | puntero[pos_sh];
							pos_sh=pos_sh+4;
							sh_size = 	(puntero[pos_sh+3] << 24) | (puntero[pos_sh+2] << 16) | (puntero[pos_sh+1] << 8) | puntero[pos_sh];
							pos_sh=pos_sh+4;
							sh_link = 	(puntero[pos_sh+3] << 24) | (puntero[pos_sh+2] << 16) | (puntero[pos_sh+1] << 8) | puntero[pos_sh];
							pos_sh=pos_sh+4;
							sh_info = 	(puntero[pos_sh+3] << 24) | (puntero[pos_sh+2] << 16) | (puntero[pos_sh+1] << 8) | puntero[pos_sh];
							pos_sh=pos_sh+8;	// Nos saltamos sh_addralign
							sh_entsize =(puntero[pos_sh+3] << 24) | (puntero[pos_sh+2] << 16) | (puntero[pos_sh+1] << 8) | puntero[pos_sh];
							
							//printf("%d %d %d %d %d\n", sh_offset, sh_size, sh_link, sh_info, sh_entsize);
							
							
							pos_r = sh_offset;							// Trabajaremos con las relocalizaciones
							pos_r_ini = pos_r;
							printf(">> seccion relocalizadores ( ):\n");
							
							for (contador_r=0; contador_r<(sh_size/sh_entsize); contador_r++, pos_r=pos_r_ini+sh_entsize, pos_r_ini=pos_r)
							{
								// if r_info == 514 (0x0202)  -- r_info tiene un offset inicial de 4 bytes respecto a r_offset, de ah� el  4-7 en lugar del t�pico 0-3
								if ( ( (puntero[pos_r+7] << 24) | (puntero[pos_r+6] << 16) | (puntero[pos_r+5] << 8) | puntero[pos_r+4]) == 514)
								{
									r_offset =	(puntero[pos_r+3] << 24) | (puntero[pos_r+2] << 16) | (puntero[pos_r+1] << 8) | puntero[pos_r];
									printf(">>> rel %d (R_ARM_ABS32): 0x%X\n", contador_r, r_offset);
									
									
									
									destino = (unsigned int*)((int)0x02100000 + r_offset - p_paddr);
									//_gm_relocatar((unsigned int*)r_offset, puntero, p_paddr);	// llamar funci�n relocalizadora
									
									
									//_gm_relocatar(destino, puntero, p_paddr);	// llamar funci�n relocalizadora
									_gm_relocatar(destino, puntero, p_paddr);	// llamar funci�n relocalizadora
									
									
									//puntero[(int)destino] = (int)0x02100000 + puntero[(int)destino] - p_paddr ;
									
								}	//endif r
								
							}	// endfor r
							
						}	// endif sh
						
					}	// endfor sh
					
				}	// endif p
				
			}	// endfor p
			
			
			free(puntero);
			result = (int)0x02100000 + entry - p_paddr;									// La funci�n debe devolver esta direcci�n de arranque
			
		}
		
		fclose(inf);
		
	}
	

	return ((intFunc) result);
}



