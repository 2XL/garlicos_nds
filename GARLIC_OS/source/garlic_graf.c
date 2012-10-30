/*------------------------------------------------------------------------------

	"garlic_graf.c" : fase 1 / programador G

	Rutinas de gestión del entorno gráfico (ventanas de texto), para el sistema
	GARLIC 0.5

------------------------------------------------------------------------------*/
#include <nds.h>

#include <garlic_system.h>	// definición de funciones y variables de sistema
#include <garlic_font.h>	// definición gráfica de caracteres
int bg2A_id; // la funció li falta un altre parametre...
 //FUNCIONS AUXILIARS...
void _gg_zocaloScroll ();
void _gg_initMarco();

/* Inicializaciones gráficas del sistema Garlic */
//------------------------------------------------------------------------------
void _gg_iniGraf()
//------------------------------------------------------------------------------
{
	// Declaracio de Variables
		int bg3A_id;
		int scale = 1<<8; // que en sistema de coma fija es 256 para el caso de 8 en factor de escala.
 
	// inicializar el procesador gráfico ?§principal en modo 5:
		videoSetMode(MODE_5_2D ); //	OK
		vramSetBankA(VRAM_A_MAIN_BG );	// OK
		 
	// inicializar los fondos gráficos 2 y 3 en modo Extended Rotation, con un tamaño de 512x512 píxeles,marcoFondo3
		bg2A_id = bgInit(2, BgType_ExRotation, BgSize_ER_512x512, 12, 3);	// retorna id fondo 2A
		bg3A_id = bgInit(3, BgType_ExRotation, BgSize_ER_512x512, 6, 3);	// retorna id fondo 3A
						//	int layer -  BgType type - BgSize - int mapBase - int tileBase);	
		bgSetPriority(bg3A_id, 0);		// fijar el fondo 3 como más prioritario que el fondo 2,
		bgSetPriority(bg2A_id, 1);

		dmaCopy(garlic_fontTiles, bgGetGfxPtr(bg3A_id), garlic_fontTilesLen);		// copiar las baldosas y la paleta para la fuente de letras
		dmaCopy(garlic_fontPal, BG_PALETTE, garlic_fontPalLen);						// los palettes :> colores

	// generar los marcos de las ventanas de texto e??n el fondo 3,
		_gg_initMarco(&bg3A_id); // max 4..., mem reserv = max 4 marco... // quedaria bé pasar per parametre lo punter del marco pero aqui suposem que sempre sera el 3.

	// escalar los fondos 2 y 3 para que quepan en la pantalla superior de la NDS.
		bgSetScale(bg3A_id, (scale)*2,(scale)*2); // massa zoooom perjudica la vista potser que m'hagi sortit del rang
		bgSetScale(bg2A_id, (scale)*2,(scale)*2); 

	//	bgSetScroll(bg2A_id, 256 ,192);		// desplaçar el fons de la pantalla 2 a baix-dret
bgUpdate();
// scale eix x e eix y
}


/* dibujar nº zocalos en el marco de fondo 3, como rango de 1 hasta 4,
	numZocalos Fila local (entre 1 i 2) (offset implica numFilaZocalo) 
	pasar la matriz por parametro...									*/
void _gg_initMarco(int *bg_id)	
{ 
	u16* buffer = (u16*)bgGetMapPtr(*bg_id);
 
 int ix = 0;
	for (ix = 0; ix< 64; ix ++)
	{
	buffer[ix] = 99;
	buffer[ix+64*23] = 97;
	buffer[ix+64*24] = 99;
	buffer[ix+64*47] = 97;
	}
	
 int iy = 0;
	for (iy = 0; iy <48;  iy++)
	{
	buffer[iy*64] = 96;
	buffer[31+iy*64] = 98;
	buffer[32+iy*64] = 96;
	buffer[63+iy*64] = 98;
	}
 // colocar els costats dels marcos
 int i = 0;
 
 while (i!=2)
	{
	int offset = 64*24*i;
 
	buffer[0			+offset] = 103;
	buffer[31			+offset] = 102;
	buffer[64*23		+offset] = 100;
	buffer[64*23 + 31	+offset] = 101;
 
	buffer[0			+offset+32] = 103;
	buffer[31			+offset+32] = 102;
	buffer[64*23		+offset+32] = 100;
	buffer[64*23 + 31	+offset+32] = 101;
 
	i++;
	}	
}

/* Escribir un mensaje de texto, como vector de caracteres terminado por cero,
	en la ventana correspondiente al zócalo indicado por parámetro */
//------------------------------------------------------------------------------
void _gg_escribir(char *mensaje, int zocalo)
//------------------------------------------------------------------------------
{
	u16* buffer = (u16*)bgGetMapPtr(bg2A_id);
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
					buffer[offset +  indexY*64 + aux] = _gd_psv[zocalo].pChars[aux]-32;	// -32 perque les tiles estan desfaçades respecte ASCI
				j = 0;
				 	
				indexY++;
				}		
		}
		
		// actualitzar el index de pControl ! fin de fase!!!
		
		pnChars = j;
		pFila = (indexY) << 16;
		_gd_psv[zocalo].pControl = (pFila|pnChars);

}

/* realizar un Scroll i borrar la ultima linea */
void _gg_zocaloScroll (int offset, u16* buffer)
{
 int x, y;
 for ( x = 0; x < 32 ; x++)
  for ( y = 0; y < 24 ; y++)
	{
	buffer[offset + x + y*64] = buffer[ offset + x + (y+1) * 64];
	}
}



