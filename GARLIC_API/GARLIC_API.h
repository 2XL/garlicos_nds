/*------------------------------------------------------------------------------

	"GARLIC_API.h" : cabeceras de funciones del API (Application Program
					Interface) del sistema operativo GARLIC (c�digo fuente
					disponible en "GARLIC_API.s")

------------------------------------------------------------------------------*/
#ifndef _GARLIC_API_h_
#define _GARLIC_API_h_


	/* GARLIC_random: devuelve un n�mero aleatorio de 32 bits */
extern int GARLIC_random();

	/* GARLIC_divmod: calcula la divisi�n num / den (numerador / denominador),
		almacenando el cociente y el resto en las posiciones de memoria indica-
		das por *quo y *mod, respectivamente (pasa resultados por referencia);
		la funci�n devuelve 0 si la divisi�n �s correcta, o diferente de 0
		si hay alg�n problema (divisi�n por cero).
		ATENCI�N: s�lo procesa n�meros naturales de 32 bits SIN signo. */
extern int GARLIC_divmod(unsigned int num, unsigned int den,
							unsigned int * quo, unsigned int * mod);

	/* GARLIC_num2str: convertir el n�mero pasado por valor en el par�metro num
		a una representaci�n en c�digos ASCII de los d�gitos decimales corres-
		pondientes, que se escribir�n dentro del vector de caracteres numstr,
		que se pasa por referencia; el par�metro length indicar� la longitud del
		vector; esta funci�n coloca un caracter centinela (cero) en la �ltima
		posici�n del vector (numstr[length-1]) y, a partir de la pen�ltima
		posici�n, empieza a colocar los c�digos ASCII correspondientes a las
		unidades, decenas, centenas, etc.; en el caso que despu�s de trancribir
		todo el n�mero todav�a queden posiciones libres en el vector, la funci�n
		rellenar� dichas posiciones con espacios en blanco, y devolver� un cero;
		en el caso que NO hayan suficientes posiciones para transcribir todo el
		n�mero, la funci�n abandonar� el proceso y devolver� un valor diferente
		de cero.
		ATENCI�N: s�lo procesa n�meros naturales de 32 bits SIN signo. */
extern int GARLIC_num2str(char * numstr, unsigned int length, unsigned int num);

	/* GARLIC_print: escribir string en la ventana del proceso actual */
extern void GARLIC_print(char * string);



#endif // _GARLIC_API_h_
