/*
 * pcb.h
 *
 *  Created on: 29/5/2017
 *      Author: utnso
 */

#ifndef PCB_H_
#define PCB_H_

typedef struct{
	int32_t		pagina; // Puede ser 0 la página. Por eso no puse u_int32_t.
	u_int32_t 	offset;
	u_int32_t 	size;
} posicionMemoria;

// Estructura para manejar una posición de memoria.

typedef struct{
	int32_t 	pid;
	int32_t		pagina; // Puede ser 0 la página. Por eso no puse u_int32_t.
	u_int32_t 	offset;
	u_int32_t 	size;
} solicitudMemoria;

// Para solicitar instrucciones en Memoria por parte de un proceso.

typedef struct{
	char 				nombre;
	posicionMemoria 	posicionMemoria; // Dice Scarcella que poner estructuras anidadas es un mal diseño... Lo cambiaría, pero denme el OK.
} variableStack;

// Estructura de la lista de variables que hay en el stack.

typedef struct{
	u_int32_t offset;	// Desplazamiento de la instrucción respecto del inicio del programa.
	u_int32_t longitud;	// Tamaño en bytes de la instrucción.
} lineaUtil;

// Líneas de código que se pueden parsear correctamente. Surgen del preprocesado del código fuente del programa.

typedef struct{
	u_int32_t	primerInstruccion;	// El numero de la primera instrucción (Begin).
	u_int32_t	instruccionesSize;	// Cantidad de instrucciones del programa.
	lineaUtil*	instrucciones; 		// Instrucciones del programa.
} indiceDeCodigo;

// El índice de Código del PCB con las líneas útiles de código.

typedef struct{
	t_size	etiquetasSize;	// Tamaño del mapa serializado de etiquetas.
	char*	etiquetas;		// La serializacion de las etiquetas.
} indiceDeEtiquetas;

// El índice de Etiqueta del PCB para poder identificar las funciones de un programa.

typedef struct{
	t_list*				argumentos; 	// Posiciones de memoria donde se almacenan las copias de los argumentos de la función.
	t_list*				variables; 		// Identificadores y posiciones de memoria donde se almacenan las variables locales de la función.
	u_int32_t			retPos; 		// Posición del índice de código donde se debe retornar al finalizar la ejecución de la función.
	posicionMemoria		retVar; 		// Posición de memoria donde se debe almacenar el resultado de la función provisto por la sentencia RETURN.
} indiceDeStack;

// El índice de Stack del PCB para poder hacer llamadas a procedimientos con sus argumentos.

typedef struct{
	int32_t 			pid; 				// Identificador de un proceso.
	int32_t 			programCounter; 	// Program counter: indica el número de la siguiente instrucción a ejecutarse.
	int32_t 			paginasUsadas; 		// Cantidad de páginas usadas por el programa (Desde 0).

	// Perdoná, Erik. Tiene que ser así. En las estructuras fijate que ya tienen sus "listas".

	indiceDeCodigo		indiceCodigo;		// Identifica líneas útiles de un programa.
	indiceDeEtiquetas	indiceEtiqueta;		// Identifica llamadas a funciones.

	// Fin de rotura de código de Erik.

	t_list*				indiceStack; 		// Ordena valores almacenados en la pila de funciones con sus valores.
	u_int32_t 			exitCode; 			// Motivo por el cual finalizó un programa.
} pcb;

// El Process Control Block que maneja el Kernel y tiene toda la información de los procesos que ejecuta la CPU.

#endif /* PCB_H_ */
