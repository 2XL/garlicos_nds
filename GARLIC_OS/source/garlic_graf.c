


/*------------------------------------------------------------------------------

	"garlic_graf.c" : fase 2 / programador G

	Rutinas de gesti�n del entorno gr�fico (ventanas de texto), para el sistema
	GARLIC 1.0

------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include <garlic_system.h>	// definici�n de funciones y variables de sistema
#include <garlic_font.h>	// definici�n gr�fica de caracteres





  //-----------------------------------------------------------------------------------------------------------------------------------------
 // VARIABLES & CONSTANTS
//-----------------------------------------------------------------------------------------------------------------------------------------

#define VCOLS	32
#define VFILS	24			// definiciones para 16 ventanas
#define PPART	4				// n�mero de ventanas horizontales o verticales
#define PCOLS	VCOLS * PPART	// n�mero de columnas por pantalla
#define PFILS	VFILS * PPART	// n�mero de filas por pantalla
#define ZMODM	3				// m�scara para el m�dulo al n�mero de z�calo
#define ZBITS	2				// bits de desplazamiento al n�mero de z�calo
#define OCOLOR	128

const unsigned int char_colors[] = {0xF0, 0x60 , 0x40};		// parametre global que especifica el color
const char tabla[][32] =
	 {
	 { 10,  0, 51, 73, 83, 84, 69, 77, 65,  0, 47, 80, 69, 82, 65, 84, 73, 86, 79,  0, 39, 33, 50, 44, 41, 35,  0, 17, 14, 16,  0, 10},
	 {106,105,105,111,105,105,105,105,111,105,105,105,105,111,105,105,105,105,105,105,105,105,111,105,105,111,105,111,105,105,105,109},
	 {104,  0, 58,104,  0, 48, 41, 36,104, 48, 82, 79, 71,104, 36, 73, 82, 14, 41, 78, 73, 14,104, 48, 73,104, 37,104, 53, 83, 79,104},
	 {115,116,116,117,116,116,116,116,117,116,116,116,116,117,116,116,116,116,116,116,116,116,117,116,116,117,116,117,116,116,116,118},
	 {104,  0,  0,104,  0,  0,  0,  0,104,  0,  0,  0,  0,104,  0,  0,  0,  0,  0,  0,  0,  0,104,  0,  0,104,  0,104,  0,  0,  0,104},
	 {107,105,105,113,105,105,105,105,113,105,105,105,105,113,105,105,105,105,105,105,105,105,113,105,105,113,105,113,105,105,105,108} 
	 };


char digit[][8] =
	{
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x7F, 0x20, 0x20, 0x20, 0x7F, 0x20, 0},
	  {	0x20, 0x20, 0x7F, 0x7F, 0x7F, 0x20, 0x20, 0},
	  { 0, 0, 0, 0, 0, 0, 0, 0}
	};

int bg;


	//-----------------------------------------------------------------------------------------------------------------------------------------
   // FUNCIONS AUXILIARS...
  //-----------------------------------------------------------------------------------------------------------------------------------------
 
   
void _gg_zocaloScroll ();		// despla�ar el contingut d'un zocalo una fila amunt
void _gg_initMarco();			// inicialitzar els 16 marcs
void _gg_dibujarMarco();		// dibuixar un marco concret
void _gg_initGraf();			// inicialitzar la pantalla principal
void _gg_initGrafSub();			// inicialitzar la pantalla tactil
void _gg_initBaldosaAux();		// inicialitzar les baldoses de colors
void _gg_initGraficaOcupaMem();	// inicialitzar les baldoses de memoria ocupada
void _gg_dibujarTabla();
void _gg_Proves();

 


  //-----------------------------------------------------------------------------------------------------------------------------------------
 // RUTINA INICIALITZACI� GRAFICA PRINCIPAL & SECUNDARIA
//-----------------------------------------------------------------------------------------------------------------------------------------

void _gg_iniGraf()
{
	_gg_initGraf();			// inicialitzar la pantalla superior/principal
	_gg_initGrafSub();		// inicialitzar la pantalla inferior/tactil
	_gg_initMarco(16, 3);	// inicialitzar prantalla principal amb els 16 marcs, amb color vermell
	_gg_dibujarTabla();		// inicialitzar pantalla secundaria amb la taula ilustrativa
	
	_gg_Proves();			// provant funcionalitats variades
	bgUpdate();				// actualitzar la pantalla per zoom...etc.
}







/*joc de proves*/
void _gg_Proves()
{


_gg_escribirMat (15, 5,	digit, 0 , 1);				// provar rutina imprimir matriu

_gg_escribirCar (20,10, 'P'-32+128*1, 0, 5 );
_gg_escribirCar (21,11, 'r'-32+128*0, 0, 5 ); //<-- OVERFLOW s'ha de passar a short
_gg_escribirCar (22,12, 'o'-32+128*1, 0, 5 );	
_gg_escribirCar (23,13, 'g'-32+128*0, 0, 5 );
_gg_escribirCar (24,14, 'G'-32+128*1, 0, 5 );

}








/* dibujar n marcos consecutivos con el color especificado por parametro */
void _gg_initMarco(int marcos, int color)	
{ 
					 // zocalo - color //
	int index = 0;
	for(index = 0; index< marcos; index++)
		_gg_dibujarMarco(index, color);
		_gp_WaitForVBlank();
}











/* Dibujar el marco correspondiente al z�calo que se pasa por par�metro, seg�n	el color especificado */
void _gg_dibujarMarco(int z, int color)
{
	u16* buffer = ((u16*)(0x06000000 +((1<<11)*16)));	// com que la mapa comen�a apartir de 0 no cal offset
	// determinar el offset inicial local
		int iniY = z/PPART, iniX = z%PPART;
		int offset = (	iniX*	VCOLS					+
						iniY*	VCOLS*PFILS);	
	// determinar el
		int i = 0;
		int j = 0;
	// dibujar
		for(i=1; i<VCOLS-1; i++)
			{	
			buffer[offset+i] = 99+				OCOLOR*color;
			buffer[offset+PCOLS*23+i] = 97+	OCOLOR*color;
			}
		for(j=1; j<VFILS-1; j++)
			{
			buffer[offset+j*PCOLS] = 96+		OCOLOR*color;
			buffer[offset+31+j*PCOLS] = 98+	OCOLOR*color;
			}
		buffer[offset] = 103+					OCOLOR*color;
		buffer[offset+31] = 102+				OCOLOR*color;
		buffer[offset+23*PCOLS] = 100+			OCOLOR*color;
		buffer[offset+23*PCOLS+31] = 101+		OCOLOR*color;
		_gp_WaitForVBlank();
}










// inicialitzacio de la pantalla principal
void _gg_initGraf()
{

	// Declaracio de Variables
		int zoom 	= 4;				// en el cas que volguem ((*)-- || (/)++) zoom.
		int scale 	= (1<<8)*zoom; 	// que en sistema de coma fija es 256  
		
		
	// inicializar el procesador gr�fico principal en modo 5:
		videoSetMode(MODE_5_2D ); 					
		vramSetBankA(VRAM_A_MAIN_BG_0x06000000 );
	
	
	// inicializar el fondo 3 --> prioridad maxima para fondo3 --> type - size 		//	--> mapBase		(2  Kbytes) ||	--> 1	<<	11
		int bg3A_id = bgInit(3, BgType_ExRotation, BgSize_ER_1024x1024, 16, 4);	// 	--> tilebase	(16	Kbytes) ||	--> 1	<<	14
		int bg2A_id = bgInit(2, BgType_ExRotation, BgSize_ER_1024x1024, 0, 4);
		bg = bg2A_id;
		bgSetPriority(bg3A_id, 0);
		bgSetPriority(bg2A_id, 1);
		
		
	// Inicialitzar les baldoses de fuente original i de color
		int offsetSize = 1<<12;		// tamany que ocupa les baldoses
		int numCopyTiles = 0;		// comptador de numero de copies efectuades
		while(numCopyTiles!=4)		
			{ 
			// efectuar la copia a la memoria
			dmaCopy(garlic_fontTiles, bgGetGfxPtr(bg3A_id)+offsetSize*numCopyTiles, garlic_fontTilesLen);		
			// declarar variables i comptador
			u16 *buffer = ((u16*)(0x06000000+(1<<14)*4)+offsetSize*numCopyTiles); 
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
		u16 *buffer 	= ((u16*)(0x06000000+(1<<14)*4)+offsetSize*4);			// declarar punter on anir� les baldoses de graf de memoria
		u16 *baldosa119 = ((u16*)(0x06000000+(1<<14)*4)+(offsetSize/128)*119);	// cargar la baldosa 119
		int i = 0;	// indice de baldosas a copiar 96
		int j = 0;	// indice baldosa;
		int k = 0;	// indice buffer;
		
		for(i=0; i<96; i++)
			for(j=0; j<32; j++, k++)		// cada baldosa ocupa 32 short == 16 integers
				buffer[k] = baldosa119[j]; 
				
				
	// inicialitzar les paletes de colors de la pantalla principal
		dmaCopy(garlic_fontPal, BG_PALETTE, garlic_fontPalLen);						
		bgSetScale(bg3A_id, scale, scale); // massa zoooom perjudica la vista potser que m'hagi sortit del rang
		bgSetScale(bg2A_id, scale, scale);
	//	bgSetScroll(bg2A_id, 256 ,192);		// despla�ar el fons de la pantalla 2 a baix-dret
		bgUpdate();	
		_gp_WaitForVBlank();


}




// inicialitzacio de la pantalla secundaria
void _gg_initGrafSub()
{


	// inicialitzar el procesador grafic secundari modo 5 i el fondo 0 
		videoSetModeSub(MODE_5_2D | DISPLAY_BG0_ACTIVE );
		vramSetBankC(VRAM_C_SUB_BG_0x06200000);
		bgInitSub(0, BgType_Text8bpp , BgSize_T_256x256, 0,1);	


	// copiar las baldosas de fuente original, de color i las de grafico memoria 
		u16 *bufferFont 	= (u16*)(0x06000000+(1<<14)*4);
		u16 *bufferDesti 	= (u16*)(0x06200000+(1<<14)*1);
		int index = 0, dades = 32*32*19;
		while(index<dades)		// copiar les baldoses de color i les baldoses normals	
			{ 
			bufferDesti[index] = bufferFont[index];
			index++;
			}
			
	// inicialitzar les paletes de color a la pantalla secundaria		
		dmaCopy(garlic_fontPal, BG_PALETTE_SUB, garlic_fontPalLen);						
	
	// inicialitzar les barres separadores
		for(index = 0; index < 16; index++)
			_gg_dibujarSeparadoresTabla(index, 3);
		bgUpdate();
		_gp_WaitForVBlank();
}




/* Escribir un mensaje de texto, como vector de caracteres terminado por cero,
	en la ventana correspondiente al z�calo indicado por par�metro */
//------------------------------------------------------------------------------
void _gg_escribir(char *mensaje, int color, int zocalo)	
//------------------------------------------------------------------------------
{
	u16* buffer = (u16*)(0x06000000);	// como saber por donde empieza la memoria -> fondo 2 main
	int aux = _gd_psv[zocalo].pControl;

// Posici� Especifica
	int pFila = aux >> 16;				// mentres sigui superior a x caldra fer lo de despla�ar 
	int pnChars = (0x0000FFFF & aux);	

	int x =  (zocalo % PPART) * VCOLS;	// fila inicial zocalo
	int y =  (zocalo / PPART) * VFILS;	// columna inicial zocalo
	
	int offset = y * PCOLS + x;		// offset del zocalo
// Index Actual	
		 
	int indexY = pFila; 	// index de fila 	actual

	int i = 0;		 			// index del missatge; 	// index del missatge per passarlo al pChar xD
	int j = pnChars; 			// index pChar; 			// n�mero de caracters carregats al buffer per imprimir
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
				
			if(j == 32) 	// en el cas de que estigui pl� caldr� buidar-ho
				{
				
				if(indexY == VFILS)			// implica que ha arribat al maxim i no puc incrementar m�s d'offset ok!
					{
					_gg_zocaloScroll (offset, buffer);		// nom�s aplicable aquest marc en particular // passo l'origen de coordenades per lo Zocalo corresponent
					indexY--;
					}
				 			// comptador del char
				for(aux = 0  ; aux < j; aux++) 
					buffer[offset +  indexY*PCOLS + aux] = (_gd_psv[zocalo].pChars[aux]-32)+(OCOLOR*color);	// -32 perque les tiles estan desfa�ades respecte ASCI
				j = 0;
				 	
				indexY++;
				}		
		}
		
		// actualitzar el index de pControl ! fin de fase!!!
		
		pnChars = j;
		pFila = (indexY) << 16;
		_gd_psv[zocalo].pControl = (pFila|pnChars);
		_gp_WaitForVBlank();

}












/*Desplazamiento de filas en zocalor en caso de escribir*/
void _gg_zocaloScroll (int offset, u16* buffer)
{
 int x, y;
 for ( x = 0; x < VCOLS ; x++)
  for ( y = 0; y < VFILS ; y++)
	{
	buffer[offset + x + y*PCOLS] = buffer[ offset + x + (y+1) * PCOLS];
	}
 _gp_WaitForVBlank();
}




















/* Escribir una baldosa (car�cter con color) en una determinada posici�n (vx, vy) de
	la ventana correspondiente al z�calo indicado por par�metro */
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
		u16* buffer = (u16*)((0x06000000));


	// determinar el offset o la posicion de la memoria	
		int x =  (zocalo % PPART) * VCOLS;	// fila inicial zocalo
		int y =  (zocalo / PPART) * VFILS;	// columna inicial zocalo
	
		int offset = y * PCOLS + x;			// offset del zocalo
		offset = offset + vx + vy*PCOLS;
		
		
	// firmar caracter en casilla...	
		buffer[offset] = c+(color*OCOLOR);


}

/* Escribir una matriz de 8x8 caracteres en una determinada posici�n (vx, vy) de
	la ventana correspondiente al z�calo indicado por par�metro */
//------------------------------------------------------------------------------

void _gg_escribirMat (int vx, int vy, char m[][8], int color, int zocalo)
{


	// seria aprovechar escribir car? o todo a mano i m�s optimo�
	u16* buffer = (u16*)((0x06000000));


	// determinar el offset o la posicion de la memoria	
		int x =  (zocalo % PPART) * VCOLS;	// fila inicial zocalo
		int y =  (zocalo / PPART) * VFILS;	// columna inicial zocalo
	
		int offset = y * PCOLS + x;			// offset del zocalo
		offset = offset + vx + vy*PCOLS;
	
	
	// firmando...
	int i=0, j=0;
	for(i=0; i<8; i++)
	 for(j=0; j<8; j++)
		if(m[i][j]!=0) buffer[offset+j+i*PCOLS] = m[i][j]-32+(color*OCOLOR);
		
		
		
}


/* Escribe los campos b�sicos de una linea de la tabla correspondiente al
		z�calo indicado por par�metro con el color especificado; los campos
		son: n�mero de z�calo, PID, keyName y direcci�n inicial */
void _gg_escribirLineaTabla(int z, int color)
{
/*
typedef struct				// Estructura del bloque de estado de un proceso
{							// (PSB: Process Status Block)
	int PID;				//	identificador del proceso (Process IDentifier)
	int PC;					//	contador de programa (Program Counter)
	int SP;					//	putero �ltima entrada de la pila (Stack Pointer)
	int Status;				//	estado del procesador (CPSR)
	int keyName;			//	nombre en clave del proceso (cuatro chars)
	int workTicks;			//	contador de ciclos de trabajo (24 bits bajos)
							//		8 bits altos: uso de CPU (%)
	int pControl;			//	control de escritura en ventana del z�calo
							//		16 bits altos: n�mero de l�nea (0-23)
							//		16 bits bajos: caracteres pendientes (0-32)
	short pChars[32];		//	vector de 32 caracteres pendientes de escritura
							//		16 bits por entrada, indicando n�mero de
							//		baldosa correspondiente al caracter+color
} PACKED garlicPSB;

extern garlicPSB _gd_psv[16];	
*/

// 	 {104,  0,  0,104,  0,  0,  0,  0,104,  0,  0,  0,  0,104,  0,  0,  0,  0,  0,  0,  0,  0,104,  0,  0,104,  0,104,  0,  0,  0,104},
if(z!=0)
{

	
u16 *bufferLine = (u16*)0x06200000;
int offset = (z+4)*32;			// offset inicial tabla
	
	



//	numero de Zocalo

char buf[2];

sprintf(buf,"%d", z);
	
if(z<10)
		bufferLine[offset+2] =  buf[0]-32+128*color;
else
	{
		bufferLine[offset+2] =  buf[1]-32+128*color;
		bufferLine[offset+1] =  buf[0]-32+128*color;
	}
	


if(_gd_psv[z].PID!=0)
{
// numero de PID

char pid[2];


sprintf(pid,"%d", _gd_psv[z].PID);



if(_gd_psv[z].PID!=0)
	{
	if(_gd_psv[z].PID<10)
		bufferLine[offset+7] =  pid[0]-32+128*color;
	else
		{
		bufferLine[offset+7] =  pid[1]-32+128*color;
		bufferLine[offset+6] =  pid[0]-32+128*color;
		}
	}
	
	
	
if(z == 0)
		bufferLine[offset+7] =  pid[0]-32+128*color;
		





// PROGRAMA		
		
if(_gd_psv[z].PID!=0)
	{
	char* progName = (char*)_gd_psv[z].keyName;
		bufferLine[offset+9 ] =  progName[0]-32+128*color;
		bufferLine[offset+10] =  progName[1]-32+128*color;
		bufferLine[offset+11] =  progName[2]-32+128*color;
		bufferLine[offset+12] =  progName[3]-32+128*color;
 
	}
	
// Dir.ini PROGRAM 
}else
// 	 {104,  0,  0,104,  0,  0,  0,  0,104,  0,  0,  0,  0,104,  0,  0,  0,  0,  0,  0,  0,  0,  104,0,  0,  104,0,  104,0,  0,  0,  104},
//		0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31   				
		{
		bufferLine[offset+4  ] = ' '-32;
		bufferLine[offset+5  ] = ' '-32;
		bufferLine[offset+6  ] = ' '-32;
		bufferLine[offset+7  ] = ' '-32;
		bufferLine[offset+9  ] = ' '-32;
		bufferLine[offset+10 ] = ' '-32;
		bufferLine[offset+11 ] = ' '-32;
		bufferLine[offset+12 ] = ' '-32;
		bufferLine[offset+14 ] = ' '-32;
		bufferLine[offset+15 ] = ' '-32;
		bufferLine[offset+16 ] = ' '-32;
		bufferLine[offset+17 ] = ' '-32;
		bufferLine[offset+18 ] = ' '-32;		
		bufferLine[offset+19 ] = ' '-32;
		bufferLine[offset+20 ] = ' '-32;
		bufferLine[offset+21 ] = ' '-32;
		bufferLine[offset+23 ] = ' '-32;
		bufferLine[offset+24 ] = ' '-32;
		bufferLine[offset+26 ] = ' '-32;
		bufferLine[offset+28 ] = ' '-32;
		bufferLine[offset+29 ] = ' '-32;
		bufferLine[offset+30 ] = ' '-32;
	
		
		}
}


}















/* Dibuja las separaciones de la linea de la tabla correspondientes al z�calo que se pasa por par�metro, con el color especificado */
void _gg_dibujarSeparadoresTabla(int z, int color)
{



	u16 *bufferLine = (u16*)0x06200000;
	int offset = (z+4)*32;
	



	int i = 0;
	for(i=0; i<32; i++)	
		if(tabla[4][i] != 0)
			bufferLine[offset+i] = tabla[4][i]+color*OCOLOR;
			



	char buf[2];


	sprintf(buf,"%d", z);
	
	if(z<10)
			bufferLine[offset+2] =  buf[0]-32+128*color;
	else
		{
			bufferLine[offset+2] =  buf[1]-32+128*color;
			bufferLine[offset+1] =  buf[0]-32+128*color;
		}
		
// 	 {104,  0,  0,104,  0,  0,  0,  0,104,  39,  33,  50,  44,104,  0,  0,  0,  0,  0,  0,  0,  0,104,  0,  0,104,  0,104,  0,  0,  0,104},
//107 ?
}





















/* Dibujar la tabla de procesos */
void _gg_dibujarTabla()
{
	int i=0, j=0, k=0; 
	// inicialitzar la tabla de la pantalla sub	
		u16 *subuffer =  (u16*)0x06200000;	
		k=0;i=0; i=0;
		
		for(i=0;	i<32;	i++, k++)
			subuffer[k] = tabla[j][i]+128; 
		
		
		for(j=1; 	j<5; 	j++)
			for(i=0;	i<32;	i++, k++)
				subuffer[k] = tabla[j][i];
		
		k=k+32*15;
		for(i=0; i<32;	i++, k++)	
			subuffer[k] = tabla[j][i]; 
			
		k=32*4+9;
		
		subuffer[k+0]=39;	// g
		subuffer[k+1]=33;	// a
		subuffer[k+2]=50;	// r
		subuffer[k+3]=44;	// l
	
	k=32*21;
		for(j=0; j<3; j++)
			for(i=0; i<32; i++,k++)
				subuffer[k]	= 128*4+i+j*32;	
}








































/**/