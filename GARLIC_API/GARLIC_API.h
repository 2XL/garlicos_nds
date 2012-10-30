/*------------------------------------------------------------------------------

	"GARLIC_API.h" : cabeceras de funciones del API (Application Program
					Interface) del sistema operativo GARLIC (código fuente
					disponible en "GARLIC_API.s")

------------------------------------------------------------------------------*/
#ifndef _GARLIC_API_h_
#define _GARLIC_API_h_


	/* GARLIC_random: devuelve un número aleatorio de 32 bits */
extern int GARLIC_random();

	/* GARLIC_divmod: calcula la división num / den (numerador / denominador),
		almacenando el cociente y el resto en las posiciones de memoria indica-
		das por *quo y *mod, respectivamente (pasa resultados por referencia);
		la función devuelve 0 si la división és correcta, o diferente de 0
		si hay algún problema (división por cero).
		ATENCIóN: sólo procesa números naturales de 32 bits SIN signo. */
extern int GARLIC_divmod(unsigned int num, unsigned int den,
							unsigned int * quo, unsigned int * mod);

	/* GARLIC_num2str: convertir el número pasado por valor en el parámetro num
		a una representación en códigos ASCII de los dígitos decimales corres-
		pondientes, que se escribirán dentro del vector de caracteres numstr,
		que se pasa por referencia; el parámetro length indicará la longitud del
		vector; esta función coloca un caracter centinela (cero) en la última
		posición del vector (numstr[length-1]) y, a partir de la penúltima
		posición, empieza a colocar los códigos ASCII correspondientes a las
		unidades, decenas, centenas, etc.; en el caso que después de trancribir
		todo el número todavía queden posiciones libres en el vector, la función
		rellenará dichas posiciones con espacios en blanco, y devolverá un cero;
		en el caso que NO hayan suficientes posiciones para transcribir todo el
		número, la función abandonará el proceso y devolverá un valor diferente
		de cero.
		ATENCIóN: sólo procesa números naturales de 32 bits SIN signo. */
extern int GARLIC_num2str(char * numstr, unsigned int length, unsigned int num);

	/* GARLIC_print: escribir string en la ventana del proceso actual, con el
		color especificado por parámetro (0 -> blanco, 1 -> amarillo, 2 -> verde,
		1 -> rojo) */
extern void GARLIC_print(char * string, int color);

	/* GARLIC_printchar: escribir un carácter (c) en la posición (vx, vy) de la
		ventana del proceso actual, donde (vx) tiene rango [0..31] y (vy) tiene
		rango [0..23], con el color especificado por parámetro (0 -> blanco,
		1 -> amarillo, 2 -> verde, 1 -> rojo) */
extern void GARLIC_printchar(int vx, int vy, char c, int color);

	/* GARLIC_printmat: escribir una matriz de carácteres (m) en la posición
		(vx, vy) de la ventana del proceso actual, donde (vx) tiene rango
		[0..31] y (vy) tiene rango [0..23], con el color especificado por
		parámetro (0 -> blanco,	1 -> amarillo, 2 -> verde, 1 -> rojo);
		 la matriz consistirá en 8x8 posiciones	con códigos ASCII, aunque las
		posiciones que contengan un código inferior a 32 (espacio en blanco) no
		modificarán las casillas correspondientes de la ventana. */
extern void GARLIC_printmat(int vx, int vy, char m[][8], int color);

	/* GARLIC_delay: retardar la ejecución del proceso actual el número de
		segundos que se especifica por el parámetro (nsec); el rango permitido
		será de [0..600] (max. 10 minutos); el valor 0 provocará que el proceso
		se desbanque y pase a READY, sin determinar un retardo concreto (el que
		resulte de volver a restaurar el proceso). */
extern void GARLIC_delay(unsigned int nsec);


	/* GARLIC_clear: borra todo el contenido de la ventana del proceso que
		invoca esta función. La siguiente llamada a la función GARLIC_print
		empezará a escribir a partir de la primera fila de la ventana. */
extern void GARLIC_clear();


#endif // _GARLIC_API_h_
