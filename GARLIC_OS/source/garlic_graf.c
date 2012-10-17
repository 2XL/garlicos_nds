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
	_gg_initMarco();
	
	// inicializar el procesador gr�fico ?�principal en modo 5:
		videoSetMode(MODE_5_2D |  DISPLAY_BG3_ACTIVE |  DISPLAY_BG2_ACTIVE); //	OK

	//reservar el banco de memoria de v�deo A,
		vramSetBankA(VRAM_A_MAIN_BG );	// OK
		 
	// inicializar los fondos gr�ficos 2 y 3 en modo Extended Rotation, con un tama�o de 512x512 p�xeles,marcoFondo3
		bg2A_id = bgInit(2, BgType_ExRotation, BgSize_ER_512x512, 12, 3);	// retorna id fondo 2A
		bg3A_id = bgInit(3, BgType_ExRotation, BgSize_ER_512x512, 6, 3);	// retorna id fondo 3A
						//	int layer -  BgType type - BgSize - int mapBase - int tileBase);	
	
		bgSetPriority(bg3A_id, 0);		// fijar el fondo 3 como m�s prioritario que el fondo 2,
		bgSetPriority(bg2A_id, 1);

		dmaCopy(garlic_fontTiles, bgGetGfxPtr(bg3A_id), garlic_fontTilesLen);		// copiar las baldosas y la paleta para la fuente de letras
		dmaCopy(garlic_fontPal, BG_PALETTE, garlic_fontPalLen);						// los palettes :> colores

	// generar los marcos de las ventanas de texto e??n el fondo 3,
		_gg_initMarco(4); // max 4..., mem reserv = max 4 marco... // quedaria b� pasar per parametre lo punter del marco pero aqui suposem que sempre sera el 3.
		dmaCopy(marcoFondo3, bgGetMapPtr(bg3A_id), sizeof(marcoFondo3));
		dmaCopy(marcoFondo2, bgGetMapPtr(bg2A_id), sizeof(marcoFondo2));
	
	// escalar los fondos 2 y 3 para que quepan en la pantalla superior de la NDS.
		bgSetScale(bg3A_id, (scale)*2,(scale)*2); // massa zoooom perjudica la vista potser que m'hagi sortit del rang
		bgSetScale(bg2A_id, (scale)*2,(scale)*2); 

	//	bgSetScroll(bg2A_id, 256 ,192);		// despla�ar el fons de la pantalla 2 a baix-dret


// scale eix x e eix y
_gp_WaitForVBlank();
bgUpdate();
}


/* dibujar n� zocalos en el marco de dondo 3, como rango de 1 hasta 4,
	numZocalos Fila local (entre 1 i 2) (offset implica numFilaZocalo) */
void _gg_initMarco(int numZocalos)
{ 
	int indexActual = 0;	// contador del dibujo zocalo...

	int indexX = 0, indexY = 0;
	int findexX = indexX + 63;
	int findexY = indexY + 23;

	while (numZocalos != indexActual)
		{
		if(indexActual % 2 == 0)	// actualitzar indexX i indexY
			{
			indexX =  0;	
			indexY =  (indexActual / 2) * 24;
			findexX =  62;
			findexY = indexY + 23;
			}
		else
			{
			indexY = (indexActual / 2) * 24;
			findexY = indexY + 23;
			indexX =  64;	
			findexX =  	findexX + 64;
			}

	// Actualitzar comptadors
		marcoFondo3[indexY][indexX] = 103;
		marcoFondo3[findexY][indexX] = 100;
		marcoFondo3[indexY][findexX] = 102;
		marcoFondo3[findexY][findexX] = 101;
		
	// inicialitzar fila corresponent
		for( indexX = indexX+2; indexX<findexX; indexX =indexX+2)	// per no tocar l'ultim ni el primer "tile"
			{
			marcoFondo3[indexY][indexX] = 99;
			marcoFondo3[findexY][indexX] = 97;
			}
		
		indexX = indexX - 62; // restablir l'offset	
	
	// inicialitzar columna corresponent
		for ( indexY = indexY+1; indexY<findexY; indexY=indexY +1) // per no tocar l'ultim ni el primer "tile"
			{
			marcoFondo3[indexY][indexX] = 96;
			marcoFondo3[indexY][findexX] = 98;
			}

		indexActual++;
		}
}


















/* Escribir un mensaje de texto, como vector de caracteres terminado por cero,
	en la ventana correspondiente al z�calo indicado por par�metro */
//------------------------------------------------------------------------------
void _gg_escribir(char *mensaje, int zocalo)
//------------------------------------------------------------------------------
{

}

