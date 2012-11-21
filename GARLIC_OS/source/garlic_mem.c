/*------------------------------------------------------------------------------

	"garlic_mem.c" : fase 1 / programador M

	Rutinas de carga de un fichero ejecutable en formato ELF, para el sistema
	GARLIC 0.5

------------------------------------------------------------------------------*/
#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <garlic_system.h>	// definición de funciones y variables de sistema

/* llegirWordsYHalfWords: a partir de un vector que contiene todos los datos, en nuestro caso
un fitxero.elf, el indice desde donde queremos comenzar a cojer el word o el half worden este vector, i
los saltos que queremos dar si es un word 4 si e sun half 2. Nos permite retornar un word o half. */
int llegirWordsYHalfWords (char *entFile, int index, int saltos){ /*Funcio pera llegir Words o Half*/

int i; /*iterador del bucle*/
int vector_paralel, retorn=0; /*variables auxiliars per a fer el desplaçament dels bits*/


	for(i=index; i<index+saltos; i++) /* bucle per a anar desde la poscio index on comença el word fins a 4 mes enlla*/
	{
		vector_paralel = entFile[i];     /*guardem la posicio i en un vector copia anomenat vector_paralel*/
		vector_paralel <<= 8*(i-index); /*desplaçament de vits a l'esquerra*/
		retorn |= vector_paralel;	  /* retornem el resultat que ens interesa*/
	}
	return(retorn);


}

/* _gm_relocatar: a partir de una dirección de memoria (destino), de un vector
			que contiene todo el fichero ELF (file) y de una dirección 'virtual'
			de inicio de un segmento (pAddr), esta función interpretará los
			'relocs' del fichero ELF y ajustará las direcciones de memoria
			correspondientes a relocs de tipo R_ARM_ABS32, restando la dirección
			virtual y sumando la dirección destino */
void _gm_relocatar(unsigned int *destino, char *file, unsigned int pAddr)
{
	int e_shentsize=llegirWordsYHalfWords(file,46,2);/* guardem el tamañ de cada entrada de la taula de seccions*/
	//iprintf("%i tamañ de cada entrada de la taula de seccions\n",e_shentsize);
	//int e_shnum=llegirWordsYHalfWords(file,48,2); /* guardem el nuemro d'entradas de la taula de seccions*/
	//printf("%i nuemro d'entradas de la taula de seccions\n",e_shnum);
	//int tamany_taulaseccions=e_shentsize*e_shnum; /*multipliquem el tamany de cada entrada pel nuemro de entrades i tenim el tamany de la taula de seccions*/
	//iprintf("%i tamany de la taula de seccions\n",tamany_taulaseccions);
	int e_shoff=llegirWordsYHalfWords(file,32,4); /*guardamos el shoffset que es la posicion de la taula de secciones*/
	//iprintf("%i posicion de la taula de secciones\n",e_shoff);
	int i;
	for(i=0;i<e_shentsize;i++){ //bucle per a accedir a tote sles seccions de la taula de seccions
		int sh_type;
		sh_type=llegirWordsYHalfWords(file,e_shoff+(i*e_shentsize)+4,4);//llegim sh_type per veure si es de tipus 9
		if (sh_type==9){ //comprovem si es de tipus 9 el que ens interesa
			int sh_offset; 
			sh_offset=llegirWordsYHalfWords(file,e_shoff+(i*e_shentsize)+16,4); //llegim sh_offset
			//iprintf("%x \n",sh_offset);
		
			int sh_size;
			sh_size=llegirWordsYHalfWords(file,e_shoff+(i*e_shentsize)+20,4);//llegim sh_size per calcular els relocs
			int sh_entsize;
			sh_entsize=llegirWordsYHalfWords(file,e_shoff+(i*e_shentsize)+36,4);//llegim sh_entsize per calcular els relocs
			int relocs=sh_size/sh_entsize;// calculem el numero de relocalitzadors a partir de la mida total i la de cada entrada
			//iprintf("%i Num Relocs: \n",relocs); //numero de relocalitzadors
			iprintf(">> seccion relocalizadores (2)\n");
			for(i=0;i<(relocs);i++){ //bucle per a accedir a tots els relocalitzadors
					
				int r_offset=llegirWordsYHalfWords(file,sh_offset+(i*8),4);//llegir el r_offset del reloc
				//iprintf("%x  ESTAAA \n",r_offset);
				int r_info=llegirWordsYHalfWords(file,sh_offset+(i*8)+4,4); //llegir el r_info del reloc
				//iprintf("%x  ESTAAA \n",r_info);
				r_info=r_info& 0x00000FF; //agafar els 8 bits baixos del r_info per veure si es de tipus 2,o R_ARM_ABS32
				if(r_info==2){ //comprovem si r_info es de tipus 2
					iprintf(">>> rel %i (R_ARM_ABS32): 0x%x\n",i,r_offset);
					int dirrec = ((unsigned int) destino) + r_offset - pAddr; // obtener la direccion de memoria a relocalizar
					//iprintf("\n Direccion a relocalizar es %x  \n", dirrec);
					int contngut_dirrec = *((unsigned int *) dirrec);
					//iprintf("\n el contenido de la dirección es %x  \n", contngut_dirrec);
					int relocalizacion = ((unsigned int) destino) + contngut_dirrec - pAddr;
					*((unsigned int *) dirrec) = relocalizacion;
					//iprintf("\n relocalizacion es %x  \n", relocalizacion);
				}
						
					
			}

		}
		
	}
}



/* _gm_cargarPrograma: cargar el programa "(keyName).elf" en una zona de memoria
			libre y relocatar las referencias a los símbolos del programa; */
//---------------------------------------------------------------------------------
intFunc _gm_cargarPrograma(char *keyName) {
//---------------------------------------------------------------------------------
	unsigned int result = 0;
	char ruta[19]="Programas/"; /*una taula de caracters amb la part de la ruta que no vaira*/
	strcat(ruta, keyName); /* concadenacio de strings o taula de caracters de la ruta amb el keyName*/
	strcat(ruta, ".elf"); /* una altra concadenacio per a poder afegirla extensio el .elf*/
	FILE* inf = fopen(ruta,"rb"); /* obrir el arxiu en mode rb read bytes*/
	if(inf) /* si s'ha obert be es a dir existeix el ficher llavors fem lo que toca*/
	{
				int len; /* enter per a enmagatzemar la llargada del fitxer*/
				fseek(inf,0,SEEK_END);
				len = ftell(inf);		/*savem la mida del fitxer i la guardem a len*/
				fseek(inf,0,SEEK_SET);
				
		char *entireFile = (char*)malloc(len+1); /* reservem un espai de memoria del tamany del fitxer +1*/
		if (entireFile) /* si em pogut crear el espai de memoria */
		{
			entireFile[len] = 0;
			if(fread(entireFile,1,len,inf) != len) /* ocmprovet si s'ha fet tot be*/
			{
				iprintf("savage error reading the bytes from the file!\n");
				free(entireFile);/* si no sa fet be lliverem l'espai de memoria reservat*/
			}
			else{ /* si s'ha pogut crear tot be començem a enmagatzemar les dades que ens interesen*/
			
			/*acceder  a  la  cabecera  ELF  del  fichero,  para  obtener  la  posición  y
tamaño de la tabla de segmentos y de la tabla de secciones,
• obtener la dirección de inicio del programa e_entry,*/

				int e_entry=llegirWordsYHalfWords(entireFile,24,4); /* guardem la direccio d'entrada del programa*/
				//iprintf("%x direccio d'entrada del programa\n",e_entry);
				int e_phoff=llegirWordsYHalfWords(entireFile,28,4); /* guardamos el phoffset que es la posicio de la taula de segments*/
				//iprintf("%x desplazamiento \n",e_phoff);
				int e_phentsize=llegirWordsYHalfWords(entireFile,42,2); /* guardem el tamañ de cada entrada de la taula de segments*/
				//iprintf("%i tamañ de cada entrada de la taula de segments\n",e_phentsize);
				int e_phnum=llegirWordsYHalfWords(entireFile,44,2); /* guardem el nuemro d'entradas de la taula de segments*/
				//iprintf("%i nuemro d'entradas de la taula de segments\n",e_phnum);
				//int tamany_taulaseg=e_phentsize*e_phnum; /*multipliquem el tamany de cada entrada pel nuemro de entrades i tenim el tamany de la taula de segments*/
				//iprintf("%i tamany de la taula de segments\n",tamany_taulaseg);
				iprintf("> cabeceras de programa %i:\n",e_phnum);
				
				/*acceder a la tabla de segmentos; para cada segmento de tipo PT_LOAD:*/
				int i=0;
				for(i=0;i<e_phentsize;i++){
					int p_type;
					p_type=llegirWordsYHalfWords(entireFile,e_phoff+(i*e_phentsize),4);
					if (p_type==1){
						//iprintf("HOLAAAAAAAAAAA \n");
						iprintf("> segmento num. %i copiado:\n",i);
						//Cargar todo lo que necesitamos p_paddr,p_offset,p_filesz,p_memsize
						int p_paddr=llegirWordsYHalfWords(entireFile,e_phoff+(i*e_phentsize)+12,4);
						int p_offset=llegirWordsYHalfWords(entireFile,e_phoff+(i*e_phentsize)+4,4);
						int p_filesz=llegirWordsYHalfWords(entireFile,e_phoff+(i*e_phentsize)+16,4);
						int p_memsz=llegirWordsYHalfWords(entireFile,e_phoff+(i*e_phentsize)+20,4);
						iprintf("    despl. en fichero: %i \n",p_offset);
						iprintf("    tam. en fichero: %i\n",p_filesz);
						iprintf("    tam. en memoria: %i \n",p_memsz);
						//iprintf("%x \n",p_paddr);
						//iprintf("%x \n",p_filesz);
						dmaCopy (&entireFile[p_offset],(void*)0x02100000, p_filesz); //copiar en la direccion destino todo el contenido del .elf
						_gm_relocatar((unsigned int *)0x02100000, entireFile,p_paddr);
						result=0x02100000+e_entry-p_paddr;
				
					}
				else{
				}
				}
				
				
				}
				
		}
		fclose(inf); /*tancar el fitxer*/
	}
	return ((intFunc) result);
}



