


/*------------------------------------------------------------------------------

	"garlic_graf.c" : fase 2 / programador G

	Rutinas de gestión del entorno gráfico (ventanas de texto), para el sistema
	GARLIC 1.0

------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include <garlic_system.h>	// definición de funciones y variables de sistema
#include <garlic_font.h>	// definición gráfica de caracteres

#define VCOLS	32
#define VFILS	24			// definiciones para 16 ventanas
#define PPART	4				// número de ventanas horizontales o verticales
#define PCOLS	VCOLS * PPART	// número de columnas por pantalla
#define PFILS	VFILS * PPART	// número de filas por pantalla
#define ZMODM	3				// máscara para el módulo al número de zócalo
#define ZBITS	2				// bits de desplazamiento al número de zócalo

const unsigned int char_colors[] = {0xF0, 0x60 , 0x40};		// parametre global que especifica el color

int bg3A_id;

								// groc  -  verd  -  vermell
 //FUNCIONS AUXILIARS...
void _gg_zocaloScroll ();		// desplaçar el contingut d'un zocalo una fila amunt
void _gg_initMarco();			// inicialitzar els 16 marcs
void _gg_dibujarMarco();		// dibuixar un marco concret
void _gg_initGraf();			// inicialitzar la pantalla principal
void _gg_initGrafSub();			// inicialitzar la pantalla tactil
void _gg_initBaldosaAux();		// inicialitzar les baldoses de colors
void _gg_initGraficaOcupaMem();	// inicialitzar les baldoses de memoria ocupada

const char tabla[][32] =
	{{ 10,  0, 51, 73, 83, 84, 69, 77, 65,  0, 47, 80, 69, 82, 65, 84, 73, 86, 79,  0, 39, 33, 50, 44, 41, 35,  0, 17, 14, 16,  0, 10},
	 {106,105,105,111,105,105,105,105,111,105,105,105,105,111,105,105,105,105,105,105,105,105,111,105,105,111,105,111,105,105,105,109},
	 {104,  0, 58,104,  0, 48, 41, 36,104, 48, 82, 79, 71,104, 36, 73, 82, 14, 41, 78, 73, 14,104, 48, 73,104, 37,104, 53, 83, 79,104},
	 {115,116,116,117,116,116,116,116,117,116,116,116,116,117,116,116,116,116,116,116,116,116,117,116,116,117,116,117,116,116,116,118},
	 {104,  0,  0,104,  0,  0,  0,  0,104,  0,  0,  0,  0,104,  0,  0,  0,  0,  0,  0,  0,  0,104,  0,  0,104,  0,104,  0,  0,  0,104},
	 {107,105,105,113,105,105,105,105,113,105,105,105,105,113,105,105,105,105,105,105,105,105,113,105,105,113,105,113,105,105,105,108}};



/* Escribe los campos básicos de una linea de la tabla correspondiente al
		zócalo indicado por parámetro con el color especificado; los campos
		son: número de zócalo, PID, keyName y dirección inicial */
void _gg_escribirLineaTabla(int z, int color)
{


}


/* Dibuja las separaciones de la linea de la tabla correspondientes al zócalo
		que se pasa por parámetro, con el color especificado */
void _gg_dibujarSeparadoresTabla(int z, int color)
{


}

/* Dibujar la tabla de procesos */
void _gg_dibujarTabla()
{


}


/* Dibujar el marco correspondiente al zócalo que se pasa por parámetro, según
	el color especificado */
void _gg_dibujarMarco(int z, int color)
{
	
	u16* buffer = (u16*)bgGetMapPtr(bg3A_id);

	
	// determinar el offset inicial
		int iniX = z/4, iniY = z%4;
		int offset = (iniX*32+iniY*128*24);	
	
	// determinar el
		int i = 0;
		int j = 0;
 
	// dibujar
		for(i=0; i<32; i++)
			{	
			buffer[offset+i] = 99+128*color;
			buffer[offset+128*23+i] = 97+128*color;
			}
		for(j=0; j<24; j++)
			{
			buffer[offset+j*128] = 96+128*color;
			buffer[offset+31+j*128] = 98+128*color;
			}
 
		buffer[offset] = 103+128*color;
		buffer[offset+31] = 102+128*color;
		buffer[offset+23*128] = 100+128*color;
		buffer[offset+23*128+31] = 101+128*color;
 

 
}

void _gg_initMarco(int marcos, int color)	
{ 
	
				// zocalo - color //
	int index = 0;
	for(index = 0; index< marcos; index++)
		_gg_dibujarMarco(index, color);

}


void _gg_iniGraf()
//------------------------------------------------------------------------------
{
	
	_gg_initGraf();			// inicialitzar la pantalla superior/principal
	_gg_initGrafSub();		// inicialitzar la pantalla inferior/tactil
	
	bgUpdate();				// actualitzar la pantalla per zoom...etc.

}

// inicialitzacio de la pantalla principal
void _gg_initGraf()
{
	// Declaracio de Variables
		
		int zoom = 4;				// en el cas que volguem ((*)-- || (/)++) zoom.
		int scale = (1<<8)*zoom; 	// que en sistema de coma fija es 256  
 
	// inicializar el procesador gráfico principal en modo 5:
		videoSetMode(MODE_5_2D ); 					
		vramSetBankA(VRAM_A_MAIN_BG_0x06000000 );	
		
	
	// inicializar el fondo 3 --> prioridad maxima para fondo3 --> type - size --> mapBase	(16 Kbytes) ||
		bg3A_id = bgInit(3, BgType_ExRotation, BgSize_ER_1024x1024, 0, 2);	// --> tilebase	(2 	Kbytes) ||
		bgSetPriority(bg3A_id, 0);													
	
	// Inicialitzar les baldoses de fuente original i de color
		int offsetSize = 1<<12;		// tamany que ocupa les baldoses
		int numCopyTiles = 0;		// comptador de numero de copies efectuades
		
		while(numCopyTiles!=4)		
			{ 
			
			// efectuar la copia a la memoria
			dmaCopy(garlic_fontTiles, bgGetGfxPtr(bg3A_id)+offsetSize*numCopyTiles, garlic_fontTilesLen);		
		
			
			// declarar variables i comptador
			u16 *buffer = (bgGetGfxPtr(bg3A_id)+offsetSize*numCopyTiles); 
			int i = 0;
			int color = char_colors[numCopyTiles-1];
			int cFF00 = color<<8 , c00FF = color, cFFFF = (cFF00|c00FF); 
		
			// assignar els colors adients
			if(numCopyTiles!=0)										
				for ( i = 0; i< offsetSize ; i++)		
					{
					if(buffer[i] != 0)
						switch (buffer[i])
							{
							case 0x00FF:	 buffer[i] = c00FF;		break;
							case 0xFF00:	 buffer[i] = cFF00;		break;
							case 0xFFFF:   	 buffer[i] = cFFFF;		break;
							}
					}
				numCopyTiles++;
			}
	// inicialitzar baldosas de grafica de memoria 		
		u16 *buffer 	= (bgGetGfxPtr(bg3A_id)+offsetSize*4);			// declarar punter o anirá les baldoses de graf de memoria
		u16 *baldosa119 = (bgGetGfxPtr(bg3A_id)+(offsetSize/128)*119);	// cargar la baldosa 119
		int i = 0;	// indice de baldosas a copiar 96
		int j = 0;	// indice baldosa;
		int k = 0;	// indice buffer;
		
		for(i=0; i<96; i++)
			for(j=0; j<32; j++, k++)
				buffer[k] = baldosa119[j]; 
	
	// inicialitzar les paletes de colors de la pantalla principal
		dmaCopy(garlic_fontPal, BG_PALETTE, garlic_fontPalLen);						
		
		_gg_initMarco(16, 3);
		
		bgSetScale(bg3A_id, scale, scale); // massa zoooom perjudica la vista potser que m'hagi sortit del rang

	//	bgSetScroll(bg2A_id, 256 ,192);		// desplaçar el fons de la pantalla 2 a baix-dret
}




// inicialitzacio de la pantalla secundaria
void _gg_initGrafSub()
{
	// variables
		int bg0C_id;
	
	// inicialitzar el procesador grafic secundari modo 5 i el fondo 0 
		videoSetModeSub(MODE_5_2D | DISPLAY_BG0_ACTIVE );
		vramSetBankC(VRAM_C_SUB_BG_0x06200000);
		bg0C_id = bgInitSub(0, BgType_Text8bpp , BgSize_T_256x256, 16,0);	
	 
	 
	// copiar las baldosas de fuente original, de color i las de grafocp memoria 
		int offsetSize = 1<<12;
		int numCopyTiles = 0;
 
 
 
 
 while(numCopyTiles!=4)		
			{ 
			
			// efectuar la copia a la memoria
			dmaCopy(garlic_fontTiles, bgGetGfxPtr(bg0C_id)+offsetSize*numCopyTiles, garlic_fontTilesLen);		
		
					// declarar variables i comptador
			u16 *buffer = (bgGetGfxPtr(bg0C_id)+offsetSize*numCopyTiles); 
			int i = 0;
			int color = char_colors[numCopyTiles-1];
			int cFF00 = color<<8 , c00FF = color, cFFFF = (cFF00|c00FF); 
		
			// assignar els colors adients
			if(numCopyTiles!=0)										
				for ( i = 0; i< offsetSize ; i++)		
					{
					if(buffer[i] != 0)
						switch (buffer[i])
							{
							case 0x00FF:	 buffer[i] = c00FF;		break;
							case 0xFF00:	 buffer[i] = cFF00;		break;
							case 0xFFFF:   	 buffer[i] = cFFFF;		break;
							}
					}
				numCopyTiles++;
			}
		
		
		u16 *buffer 	= (bgGetGfxPtr(bg0C_id)+offsetSize*4);			// declarar punter o anirá les baldoses de graf de memoria
		u16 *baldosa119 = (bgGetGfxPtr(bg0C_id)+(offsetSize/128)*119);	// cargar la baldosa 119
		int i = 0;	// indice de baldosas a copiar 96
		int j = 0;	// indice baldosa;
		int k = 0;	// indice buffer;
		
		for(i=0; i<96; i++)
			for(j=0; j<32; j++, k++)
				buffer[k] = baldosa119[j]; 
	
		
		
		
		dmaCopy(garlic_fontPal, BG_PALETTE_SUB, garlic_fontPalLen);						
		
		 
		
		u16 *subuffer =  bgGetMapPtr(bg0C_id);
		k=0;i=0; i=0;
		for(j=0; 	j<6; 	j++)
			for(i=0;	i<32;	i++, k++)
				
				if(j==0)
					subuffer[k] = tabla[j][i]+128; 
				else
					subuffer[k] = tabla[j][i]; 
		
	bgUpdate();
}




/* Escribir un mensaje de texto, como vector de caracteres terminado por cero,
	en la ventana correspondiente al zócalo indicado por parámetro */
//------------------------------------------------------------------------------
void _gg_escribir(char *mensaje, int color, int zocalo)
//------------------------------------------------------------------------------
{
u16* buffer = (u16*)VRAM_A_MAIN_BG;	// como saber por donde empieza la memoria ?
	int aux = _gd_psv[zocalo].pControl;

// Posició Especifica
	int pFila = aux >> 16;				// mentres sigui superior a x caldra fer lo de desplaçar 
	int pnChars = (0x0000FFFF & aux);	

	int x =  (zocalo % 2) * 32;	// fila inicial zocalo
	int y =  (zocalo / 2) * 24;	// columna inicial zocalo
	
	int offset = y * 64 + x;		// offset del zocalo
// Index Actual	
		 
	int indexY = pFila; 	// index de fila 	actual

	int i = 0;		 			// index del missatge; 	// index del missatge per passarlo al pChar xD
	int j = pnChars; 			// index pChar; 			// número de caracters carregats al buffer per imprimir
	// carregar pChar		
	
	while (mensaje[i]!= '\0')		// sembla que l'estigui carregant
		{
			if(j<32)				// si el buffer pChars no esta ple continuem omplint-ho
				{
				switch (mensaje[i])
					{
					case '\n':		// afegir fins que provoqui un salt de linea
						while(j<32)
							{
							_gd_psv[zocalo].pChars[j] = ' ';	// caso particular salt de linea
							j++;
							}
					break;
					case '\t':		// afegir fins que trobi el proxim multiple a 4, afegir abans de incrementar
						do
							{
							_gd_psv[zocalo].pChars[j] = ' ';	// caso particular salto tabulador == space.dim.mod: 4
							j++;
							}
						while(j%4!=0);
					break;
					default:
						_gd_psv[zocalo].pChars[j] = mensaje[i];
						j++;
					break;
					}
				}
				i++;		// incrementar l'index del msg		
				
			if(j == 32) 	// en el cas de que estigui plé caldrà buidar-ho
				{
				
				if(indexY == 24)			// implica que ha arribat al maxim i no puc incrementar més d'offset ok!
					{
					_gg_zocaloScroll (offset, buffer);		// només aplicable aquest marc en particular // passo l'origen de coordenades per lo Zocalo corresponent
					indexY--;
					}
				 			// comptador del char
				for(aux = 0  ; aux < j; aux++) 
					buffer[offset +  indexY*64 + aux] = (_gd_psv[zocalo].pChars[aux]-32)+(128*color);	// -32 perque les tiles estan desfaçades respecte ASCI
				j = 0;
				 	
				indexY++;
				}		
		}
		
		// actualitzar el index de pControl ! fin de fase!!!
		
		pnChars = j;
		pFila = (indexY) << 16;
		_gd_psv[zocalo].pControl = (pFila|pnChars);

}

void _gg_zocaloScroll (int offset, u16* buffer)
{
 int x, y;
 for ( x = 0; x < 32 ; x++)
  for ( y = 0; y < 24 ; y++)
	{
	buffer[offset + x + y*64] = buffer[ offset + x + (y+1) * 64];
	}
}


/* Escribir una baldosa (carácter con color) en una determinada posición (vx, vy) de
	la ventana correspondiente al zócalo indicado por parámetro */
//------------------------------------------------------------------------------
void _gg_escribirCar( int vx, int vy, char c, int color, int zocalo )
{
	/*	--> cada zocalo = 32 * 24 * 2 = 1536
	
	0	-	1	-	2	-	3 
	4	-	5	-	6	-	7
	8	-	9	-	10	-	11
	12	-	13	-	14	-	15	
	
	*/
	
	// determinar el puntero buffer de memoria
		u16* buffer = (u16*)VRAM_A_MAIN_BG;
	// determinar el offset o la posicion de la memoria
		int offset = (zocalo*128*24+vx + vy*128);
	// determinar el indice caracter i sumar su offset
		short caracter = (short)c-32-48;
	// escribir
		buffer[offset] = caracter+(color*128);



}

/* Escribir una matriz de 8x8 caracteres en una determinada posición (vx, vy) de
	la ventana correspondiente al zócalo indicado por parámetro */
//------------------------------------------------------------------------------

void _gg_escribirMat (int vx, int vy, char m[][8], int color, int zocalo)
{
	// seria aprovechar escribir car? o todo a mano i más optimo¿
	int i=0, j=0;
	for(i=0; i<8; i++)
	 for(j=0; j<8; j++)
		_gg_escribirCar(vx+j,vy+i, m[j][i], color, zocalo);
}


