/*------------------------------------------------------------------------------


------------------------------------------------------------------------------*/

#include <GARLIC_API.h>			/* definición de las funciones API de GARLIC */

#define ALTURA 16	/* numero de filas del laberinto;	*/
#define ANCHURA 8	/* numero de columnas del laberinto ANCHURA *arg */
#define OX 0
#define OY 4

#define RX 32		/*	limits de la pantalla */
#define RY 24
//#define RXY RX*RY

			
unsigned short Maze[RX][RY];	// total té 	768	posicions


					// inicialitzar directament la matriu amb el tamany de la pantalla 
					// cada casella correspon a una baldosa
		//X	nº de columnes
		//Y nº de files

/*
ademas de enlazar todo el codigo del sistema, 
en esta segunda fase el director tendrá que realizar un nuevo programa
para GARLIC 1.0:

se trata del programa "LABE.elf",

	que consistira en construir un laberinto aleatorio con bloques solidos y con puntos
	y con varias letras que iran recorriendo el laberinto para comerse los puntos.
	
	la siguiente figura muestra un ejemplo para tres letras, a,b,c:...

	el programa "LABE.elf" aceptara un argumento con 4 possibles valores: 0,1,2,3.
	la alturadel laberinto sera de 16 filas, anchura variables (segun arg), incluido bordes.
	la posicion de la esquina superior-izquierda sera la misma (fila, columna) / (4,0)

el numeor de bloques intermedios seran un 25% del area dispobible, 
igual que el numero de puntos.

la posicion inicial de los bloques y de los puntos sera totalmente aleatoria, lo cual puede 
generar zonas cerradas con puntos que nunca se podrán acceder (no hay que intentar evitarlo)


el numero de letras sera igual al valor del argument más uno.

cada letra se escribira con un color diferente.

la posicion inicial de las letras sera aleatoria y el algoritmo para recorrer el laberinto 
se deja libre como parte del diseño del programa. por supuesto, 
las letreas no pueden traspasar los bloques solidos ni los limites del laberinto.


en la penultima fila de la ventana se escribira el numero de puntos 
que ha conseguido cada letras hasta el momento

el programa acaba cuando se han eliminado todos los puntos. 
puede que no acabe nunca si algunos puntos quedan encerrados entre bloques solidos.


*/

typedef struct
	{
		int x;
		int y;
	} Punt;


typedef struct				 
	{							 
		Punt pos;					// actual position
		Punt vel;					// move direction
		int tIndex;					// tile index
		Punt scorePos;				// score position
		int score;					// puntuació actual
		int color;
	 
	} Foo;

Foo zoo[4];	// possibles jugadors

void _labFoo(int arg, char foo);
void _createLaberint(int arg);
void _scoreBoard(int arg);
void _labUpdate(int arg);
void _zooUpdate(Foo zoo, int arg);
int _isEnd(int arg);


// necesito una matriu de lo que genero a la memoria
// o hi ha alguna forma de obtindre?

//	START

int _start(int arg)				/* función de inicio : no se usa 'main' */
	{
		GARLIC_clear();
		GARLIC_print("-- Programa LABE (Garlic 1.0) --", 2);
		
		_createLaberint(2);
	//	_labUpdate(arg);
		return 0;
		
	}

void _createLaberint(int arg)
{
	arg = arg+1;

	int filas = ALTURA;								// num de files 16 sempre
	int columnas = arg * ANCHURA;					// num de columnes args * 8
	int nBlocks = filas * columnas * 25 / 100;		// num de blocks absoluts
	int nPuntos = 0;
	int color = arg & 3;		// asignar un color en función del argumento
	
//	int dirx, diry;
	
	int i=0,j=0; 			// contador multiuso bucle for

// INICIALITZAR LA MATRIU A 0
/*
	for(i=; i<RX; i++) 
	{
		for( j=0; j<RY; j++)
		{
			Maze[i][j] = 0;
		}
	}	
*/
// INICIALITZAR LES COLUMNES


for( i = 0, j=0; i<	filas; i++, j++)
	{
		GARLIC_printchar( OX, 				OY+j, 95, color);	
		GARLIC_printchar( OX+columnas-1,	OY+j, 95, color);
	//	Maze[OX][OY+j] = 95;
	//	Maze[OX+columnas-1][OY+j] = '95';
	}
	
//	nBlocks = nBlocks - filas*arg;

// INICIALITZAR LES FILES

for( i = 0, j = 0; j<	columnas; j++, i++)
	{
		GARLIC_printchar( OX+i, 	OY, 95, color);	
		GARLIC_printchar( OX+i,		OY+filas-1, 95, color);
//		Maze[OX+i][OY] = 95;
//		Maze[OX+i][OY+filas-1] = 95;
	}	
	nBlocks = nBlocks - columnas*arg;
	nPuntos = nBlocks;

	// las pelotas se mueven solos?
		// necesita un timer?
	i=0; // como hago aleatorio?
	 
	int eixX, eixY;
		//		 
		//	Y 			16
		//				10000
		//	X	8		16		24		32
		//		1000	10000	11000	100000
		
	int mascX= (((ANCHURA)*arg)-2);	// ANCHURA * arg - 2(parets)	
	int mascY = ALTURA-2; 				// ALTURA 		- 2(parets)

	nBlocks = mascX*mascY*25;					// MASCARAS
	nBlocks /=100;
	nPuntos = nBlocks;


// INICIALITZAR OBSTACLES

	//do
	//	{
		
		
	//		eixX = GARLIC_random()&0xFF;
	//			while(eixX>mascX) eixX -=mascX;
	//		eixY = GARLIC_random()&0xFF;	
	//			while(eixY>mascY) eixY -=mascY;
/*
			if(Maze[eixX][eixY+5] == 0)
				{
					Maze[eixX][eixY+5] = 95;
					GARLIC_printchar( eixX , eixY+5,  95, color);
					nBlocks--;
				}	
*/	//		GARLIC_printchar( eixX , eixY+4,  95, color); nBlocks--;
	//	} while (nBlocks > 0); 				// no acaba nunca

// INIT PUNTS 

	do{
			eixX = GARLIC_random()&0xFF;
				while(eixX>mascX-1) eixX -=(mascX);
			eixY = GARLIC_random()&0xFF;	
				while(eixY>mascY-1) eixY -=mascY;
/*
			if(Maze[eixX+1][eixY+5] == 0)
				{
					Maze[eixX][eixY+5] = 14;
					GARLIC_printchar( eixX , eixY+5, 14, color);
					nPuntos--;
				}
*/			GARLIC_printchar( eixX , eixY+4, 14, color); nPuntos--;
		} while (nPuntos > 0); 				// no acaba nunca


	// init player
	 
	int player_i = 0;
	do
		{
		
			eixX = GARLIC_random()&0xFF;
				while(eixX>mascX) eixX -=mascX;
			eixY = GARLIC_random()&0xFF;	
				while(eixY>mascY) eixY -=mascY;
	/*		
			if(Maze[eixX+1][eixY+5] == 0)
				{
					Maze[eixX][eixY+5] = 33+player_i;
					GARLIC_printchar( eixX+1 , eixY+5, 33+player_i, player_i);

					zoo[player_i].pos.x = eixX+1;
					zoo[player_i].pos.y = eixY+5;
					zoo[player_i].vel.x = eixX&1;
					zoo[player_i].vel.y = eixY&1;	
					zoo[player_i].tIndex = 33+player_i;
					zoo[player_i].color = player_i;
					zoo[player_i].scorePos.x = 4+player_i*7+2; // lloc on comença a imprimir puntuació
					zoo[player_i].scorePos.y = 21;
					zoo[player_i].score = 0;
			
					GARLIC_printchar((4+player_i*7), 21 , 33+player_i, player_i); 
					GARLIC_printchar((4+player_i*7)+1, 21 , 26, player_i);

					player_i++;
				}
	*/
		GARLIC_printchar( eixX+1 , eixY+5, 33+player_i, player_i); player_i++;
	
		GARLIC_printchar((4+player_i*7), 21 , 33+player_i, player_i); 
		GARLIC_printchar((4+player_i*7)+1, 21 , 26, player_i);
		
		} while (arg > player_i); 				// no acaba nunca

}







/*
 
void _labUpdate(int arg)
{

	int index = 0;
	do	{
		do	{
			GARLIC_delay(arg);
	
	// CASOS:
		// PARET:	GARLIC_printchar( OX+i,		OY+filas-1, 95, color);
		
		
		// PUNT:	GARLIC_printchar( eixX+1 , eixY+5, 14, color);
		
		
		// BUIT:	GARLIC_printchar( eixX+1 , eixY+5, 0, player_i);
		
		
		// LLETRES:	GARLIC_printchar( eixX+1 , eixY+5, 33+player_i, player_i);
			_zooUpdate(zoo[index], arg);
			index++;
			}while(index<arg);
		index=0;
		}while(_isEnd(arg)!=1);
	}

*/



/*

void _zooUpdate(Foo zoo, int arg)
	{
	
	
	typedef struct
	{
		int x;
		int y;
	} Punt;


	typedef struct				 
	{							 
		Punt pos;					// actual position
		Punt vel;					// move direction
		int tIndex;					// tile index
		Punt scorePos;				// score position
		int score;					// puntuació actual
		int color;
	 
	} Foo;
	
	
	*/
/*	
	GARLIC_printchar( zoo.pos.x, zoo.pos.y, 0, color);	
		
		// borrar caracter anterior
		zoo.pos.x += zoo.vel.x;
		zoo.pos.y += zoo.vel.y;	

	
	GARLIC_printchar( x, y, 95, color);	// reescribir caracter
	
	}

*/




/*
int _isEnd(int arg)
	{

// detectar final de partida...
// cuando hay un ganador que sume mas de la mitad de puntos possibles
// que tenga mayoria respecto lo que posse i lo que queda...
	return 0;
	}

*/











 





