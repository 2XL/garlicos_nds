/*------------------------------------------------------------------------------


	"main.c" : fase 2 / ProgG


	Programa de control del sistema operativo GARLIC, versión 1.0

------------------------------------------------------------------------------*/
#include <nds.h>
#include <filesystem.h>
#include <stdlib.h>

#include <garlic_system.h>	// definición de funciones y variables de sistema

extern int * punixTime;		// puntero a zona de memoria con el tiempo real

const short divFreq1 = -33513982/(1024*4);		// frecuencia de TIMER1 = 4 Hz

char *progs[6];
int num_progs;

int za = 0;		// índice zócalo actual
int orgX = 0, orgY = 0;		// origen de las ventanas
int orgX_ant = 0, orgY_ant = 0;		// origen anterior
int zoom = 2 << 8;				// zoom actual
int zoom_ant = 2 << 8;				// zoom anterior
int numFrames = 0;		// número de frames actual



void movimientoVentanas();
int  escogerOpcion(char *opciones[], int num_opciones);
void seleccionarPrograma();
void redibujarZocalo(int seleccionar);
void ajustarScroll();
void iniciarZoom();
void controlInterfaz();
void gestionSincronismos();
void inicializarSistema();



//------------------------------------------------------------------------------
int main(int argc, char **argv) {
//------------------------------------------------------------------------------

	inicializarSistema();
	
	_gg_escribir("********************************", 1, 0);
	_gg_escribir("*                              *", 1, 0);
	_gg_escribir("* Sistema Operativo GARLIC 1.0 *", 1, 0);
	_gg_escribir("*                              *", 1, 0);
	_gg_escribir("********************************", 1, 0);
	_gg_escribir("*** Inicio fase 2 / ProgG\n", 1, 0);

	num_progs = 4;
	progs[0] = "BORR";		// se supone que estos programas están disponibles
	progs[1] = "CRON";		// en el directorio "Programas" de las estructura
	progs[2] = "HOLA";		// de ficheros de Nitrofiles
	progs[3] = "PONG";
	while (1)						// bucle infinito
	{	controlInterfaz();
		gestionSincronismos();
		_gp_WaitForVBlank();		// retardo del proceso de sistema
	}
	return 0;			

}
























/* Función para realizar el movimiento de las ventanas en medio segundo */
void movimientoVentanas()
{
	int n_orgX, n_orgY, n_zoom, n_mask;

	numFrames--;
	n_mask = 0;
	n_orgX = orgX;
	n_orgY = orgY;
	if (orgX != orgX_ant)
	{	n_orgX -= ((orgX - orgX_ant) * numFrames) >> 5;
		n_mask |= 1;
	}
	if (orgY != orgY_ant)
	{	n_orgY -= ((orgY - orgY_ant) * numFrames) >> 5;
		n_mask |= 1;
	}
	if (n_mask & 1)
	{	bgSetScroll(2, n_orgX, n_orgY);
		bgSetScroll(3, n_orgX, n_orgY);
		if (numFrames == 0)
		{	orgX_ant = orgX;
			orgY_ant = orgY;
		}
	}
	n_zoom = zoom;
	if (zoom != zoom_ant)
	{	n_zoom -= ((zoom - zoom_ant) * numFrames) >> 5;
		n_mask |= 2;
	}
	if (n_mask & 2)
	{	bgSetScale(2, n_zoom, n_zoom);
		bgSetScale(3, n_zoom, n_zoom);
		if (numFrames == 0)
			zoom_ant = zoom;
	}
	if (n_mask) bgUpdate();
}


/* Función para presentar una lista de opciones y escoger una: devuelve el índice de la opción
		(0: primera opción, 1: segunda opción, etc.)
		ATENCIóN: para que pueda funcionar correctamente, se supone que no habrá desplazamiento
				  de las líneas de la ventana. */
int escogerOpcion(char *opciones[], int num_opciones)
{
	int fil_ini, j, sel, k;
	
	fil_ini = _gd_psv[za].pControl >> 16;		// fil_ini es índice fila inicial
	for (j = 0; j < num_opciones; j++)
	{											// mostrar opciones
		_gg_escribir("( ) ", 1, za);
		_gg_escribir(opciones[j], 1, za);
		_gg_escribir("\n", 1, za);
	}
	sel = -1;									// marca de no selección
	j = 0;										// j es preselección
	_gg_escribirCar(1, fil_ini, 10, 2, za);	// marcar preselección
	do
	{
		_gp_WaitForVBlank();
		scanKeys();
		k = keysDown();				// leer botones
		if (k != 0)
			switch (k)
			{
				case KEY_UP:
						if (j > 0)
						{	_gg_escribirCar(1, fil_ini+j, 0, 2, za);
							j--;
							_gg_escribirCar(1, fil_ini+j, 10, 2, za);
						}
						break;
				case KEY_DOWN:
						if (j < num_opciones-1)
						{	_gg_escribirCar(1, fil_ini+j, 0, 2, za);
							j++;
							_gg_escribirCar(1, fil_ini+j, 10, 2, za);
						}
						break;
				case KEY_START:
						sel = j;			// escoger preselección
						break;
			}
		if (numFrames > 0) movimientoVentanas();
	} while (sel == -1);
	return sel;
}


/* Función para permitir seleccionar un programa entre los ficheros ELF disponibles */
void seleccionarPrograma()
{
	intFunc start;

	char num[3];
	int ind_prog, i;

	for (i = 1; i < 16; i++)		// buscar si hay otro proceso en marcha
		if (_gd_psv[i].PID != 0)
		{	_gd_psv[i].PID = 0;
			_gd_nReady = 0;				// eliminar el proceso de cola de READY
			_gd_psv[i].pControl = 0;	// resetear el contador de filas y caracteres
			_gg_escribir("* ", 0, 0);
			_gi_num2str(num, 3, i);
			_gg_escribir(num, 0, 0);
			_gg_escribir(": proceso destruido\n", 0, 0);
			_gg_dibujarSeparadoresTabla(i, (i == za ? 2 : 3));
			_gg_escribirLineaTabla(i, (i == za ? 2 : 3));
			if (i != za)			// si no se trata del propio zócalo actual
				_gg_dibujarMarco(i, 3);
			break;					// abortar bucle
		}
	_gs_borrarVentana(za, 1);
	_gg_escribir("*** Seleccionar programa :\n", 1, za);
	ind_prog = escogerOpcion(progs, num_progs);
	

	start = _gm_cargarPrograma(progs[ind_prog]);
	if (start)
	{	_gp_crearProc(start, za, progs[ind_prog]);
		_gg_escribir("* ", 2, 0);
		_gi_num2str(num, 3, za);
		_gg_escribir(num, 2, 0);
		_gg_escribir(": ", 2, 0);
		_gg_escribir(progs[ind_prog], 2, 0);
		_gg_escribir(".elf\n", 2, 0);
		_gg_escribirLineaTabla(za, 2);
	}
}



/* Funcion para actualizar la tabal de zócalos en función del zócalo actual (za)
	y del parámetro (seleccionar): si seleccionar == 0 dibuja la línea de za
	según el color asociado al estado del zócalo (blanco -> activo, rojo ->
	libre); sino, dibuja la línea en verde. */
void redibujarZocalo(int seleccionar)
{
	int color;
	color = (seleccionar == 0 ? ((za == 0) || (_gd_psv[za].PID != 0) ? 0 : 3) : 2);
	_gg_dibujarSeparadoresTabla(za,  color);
	_gg_escribirLineaTabla(za, color);
	_gg_dibujarMarco(za, color);
}

/* Función auxiliar para ajustar el scroll para la ventana del zócalo actual
	(za), teniendo en cuenta su posición y el zoom actual; también reajusta un
	posible zoom activo */
void ajustarScroll()
{
	int zmod4, zdiv4;
	int d_orgX, d_orgY;

	zmod4 = za & 3;
	zdiv4 = za >> 2;
	if (zoom == 1024)
		{ d_orgX = 0; d_orgY = 0; }	// zoom máximo -> no hay scroll
	else if (zoom == 512)
		{ d_orgX = (zmod4 / 2) * 512;	// zoom intermedio -> scroll en
		  d_orgY = (zdiv4 / 2) * 384;	// 4 quadrantes
		}
	else {	d_orgX = zmod4 * 256;		// zoom mínimo -> scroll de centrado
			d_orgY = zdiv4 * 192;		// de cada ventana
		 }
	if ((d_orgX != orgX) || (d_orgY != orgY))
	{
		orgX_ant = orgX;		// iniciar un nuevo scroll
		orgY_ant = orgY;
		orgX = d_orgX;
		orgY = d_orgY;
		if (zoom_ant != zoom)	// ajustar el posible zoom activo
			zoom_ant = zoom - (((zoom - zoom_ant) * numFrames) >> 5);
		numFrames = 32;
	}
}


/* Función auxiliar para iniciar un zoom, recalculando un posible cambio de
		scroll */
void iniciarZoom()
{
	if (orgX_ant != orgX)
		orgX_ant = orgX - (((orgX - orgX_ant) * numFrames) >> 5);
	if (orgY_ant != orgY)
		orgY_ant = orgY - (((orgY - orgY_ant) * numFrames) >> 5);
	numFrames = 32;
	ajustarScroll();
}


/* Función para cambiar selección de zócalo, cambiar zoom, o iniciar proceso */
void controlInterfaz()
{
	int key;

	scanKeys();
	key = keysDown();				// leer botones
	if (key != 0)
		switch (key)
		{
			case KEY_UP:
					if (za >= 4)
					{	redibujarZocalo(0);
						za -= 4;				// subir la selección de zócalo
						redibujarZocalo(1);
						ajustarScroll();
					}
					break;
			case KEY_DOWN:
					if (za < 12)
					{	redibujarZocalo(0);
						za += 4;				// bajar la selección de zócalo
						redibujarZocalo(1);
						ajustarScroll();
					}
					break;
			case KEY_LEFT:
					if ((za % 4) >= 1)
					{	redibujarZocalo(0);
						za -= 1;				// selección de zócalo izquierdo
						redibujarZocalo(1);
						ajustarScroll();
					}
					break;
			case KEY_RIGHT:
					if ((za % 4) < 3)
					{	redibujarZocalo(0);
						za += 1;				// selección de zócalo derecho
						redibujarZocalo(1);
						ajustarScroll();
					}
					break;
			case KEY_START:
					zoom_ant = zoom;
					zoom = 1 << 8;
					iniciarZoom();
					if (za != 0)	
						seleccionarPrograma();	// iniciar programa en zócalo za
					break;
			case KEY_L:
					if (zoom > 256)
					{	zoom_ant = zoom;
						zoom >>= 1;			// dividir por 2 el factor de zoom
						iniciarZoom();		// actual (aumenta la escala)
					}
					break;
			case KEY_R:
					if (zoom < 1024)
					{	zoom_ant = zoom;
						zoom <<= 1;			// mutiplicar por 2 el factor de zoom
						iniciarZoom();		// actual (disminuye la escala)
					}
					break;
		}
	if (numFrames > 0) movimientoVentanas();
}


/* Función para gestionar los sincronismos generados por diversas rutinas
		para el programa principal */
void gestionSincronismos()
{
	int i, mask;
	char num[3];
	
	mask = 2;
	for (i = 1; i <= 15; i++)
	{
		if (_gd_sincMain & mask)
		{	// actualizar visualización de tabla de zócalos
			_gg_dibujarSeparadoresTabla(i, (i == za ? 2 : 3));
			_gg_escribirLineaTabla(i, (i == za ? 2 : 3));
			_gg_dibujarMarco(i, 3);
			_gg_escribir("* ", 3, 0);
			_gi_num2str(num, 3, i);
			_gg_escribir(num, 3, 0);
			_gg_escribir(": proceso terminado\n", 3, 0);
			_gd_sincMain &= ~mask;		// poner bit a cero
		}
		mask <<= 1;
	}
}


/* Inicializaciones generales del sistema Garlic */
//------------------------------------------------------------------------------
void inicializarSistema() {
//------------------------------------------------------------------------------

	_gd_seed = *punixTime;	// inicializar semilla para números aleatorios con
	_gd_seed <<= 16;		// el valor de tiempo real UNIX, desplazado 16 bits
	
	_gd_psv[0].keyName = 0x4C524147;	// "GARL"
	
	_gg_iniGraf();						// inicializar gráficos
	
	redibujarZocalo(1);				// marca tabla de zócalos con el proceso
									// del S.O. seleccionado (en verde)
	
	if (!nitroFSInit())
	{	_gg_escribir("ERROR: ¡no se puede utilizar NitroFS!", 3, 0);
		exit(0);
	}

	irqInitHandler(_gp_IntrMain);		// inicializar IRQs
	irqSet(IRQ_VBLANK, _gp_rsiVBL);
	irqEnable(IRQ_VBLANK);

	irqSet(IRQ_TIMER1, _gs_rsiTIMER1);
	irqEnable(IRQ_TIMER1);				// instalar la RSI para el TIMER1
	TIMER1_DATA = divFreq1; 
	TIMER1_CR = 0xC3;  		// Timer Start | IRQ Enabled | Prescaler 3 (F/1024)
}






