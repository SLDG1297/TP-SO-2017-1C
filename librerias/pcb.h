/*
 * pcb.h
 *
 *  Created on: 29/5/2017
 *      Author: utnso
 */

#ifndef PCB_H_
#define PCB_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../librerias/serializador.h"



// Estructuras de datos

// el "__attribute__((packed))" es para que la estructura no tenga padding (Espacio de más para mejorar el acceso a la info, que puede cambiar el tamaño que se quiere enviar realmente)

typedef struct{
	u_int32_t	pagina;
	u_int32_t 	offset;
	u_int32_t 	size;
} __attribute__((packed))
posicionMemoria;

// Estructura para manejar una posición de memoria.

typedef struct{
	u_int32_t 	pid;
	u_int32_t	pagina;
	u_int32_t 	offset;
	u_int32_t 	size;
} __attribute__((packed))
solicitudMemoria;

// Para solicitar instrucciones en Memoria por parte de un proceso.

typedef posicionMemoria argStack;

// Estructura de la lista de argumentos que hay en el stack.

typedef struct{
	char 				nombre;
	posicionMemoria 	posicionMemoria; // Dice Scarcella que poner estructuras anidadas es un mal diseño... Lo cambiaría, pero denme el OK.
} __attribute__((packed))
variableStack;

// Estructura de la lista de variables que hay en el stack.

typedef struct{
	u_int32_t offset;	// Desplazamiento de la instrucción respecto del inicio del programa.
	u_int32_t longitud;	// Tamaño en bytes de la instrucción.
} __attribute__((packed))
lineaUtil;

// Líneas de código que se pueden parsear correctamente. Surgen del preprocesado del código fuente del programa.

typedef struct{
	size_t				cantidadArgumentos;		// Tamaño en bytes de todos los argumentos del stack.
	t_list*				argumentos; 			// Posiciones de memoria donde se almacenan las copias de los argumentos de la función.
	size_t				cantidadVariables;		// Tamaño en bytes de todas las variables del stack.
	t_list*				variables; 				// Identificadores y posiciones de memoria donde se almacenan las variables locales de la función.
	u_int32_t			retPos; 				// Posición del índice de código donde se debe retornar al finalizar la ejecución de la función.
	posicionMemoria		retVar; 				// Posición de memoria donde se debe almacenar el resultado de la función provisto por la sentencia RETURN.
} __attribute__((packed))
indiceDeStack;

// El índice de Stack del PCB para poder hacer llamadas a procedimientos con sus argumentos.

typedef struct{
	// Campos Obligatorios

	u_int32_t 			pid; 						// Identificador de un proceso.
	u_int32_t 			programCounter; 			// Program counter: indica el número de la siguiente instrucción a ejecutarse.
	u_int32_t 			paginasUsadas; 				// Cantidad de páginas usadas por el programa (Desde 0).
	size_t				cantidadInstrucciones;		// Tamaño en bytes del ínidce de código.
	lineaUtil*			indiceCodigo;				// Identifica líneas útiles de un programa.
	size_t				cantidadEtiquetas;			// Tamaño en bytes del índice de etiquetas.
	char*				indiceEtiqueta;				// Identifica llamadas a funciones.
	size_t				cantidadStack;				// Tamaño en bytes del índice de stack.
	t_list*				indiceStack; 				// Ordena valores almacenados en la pila de funciones con sus valores.
	u_int32_t 			exitCode; 					// Motivo por el cual finalizó un programa.

	// Campos adicionales

} __attribute__((packed))
pcb;

// El Process Control Block que maneja el Kernel y tiene toda la información de los procesos que ejecuta la CPU. Su orden en la estructura determina el protocolo para enviar mensajes.



// Declaraciones

void	enviarPCB(int socket, pcb* unPcb);		// Enviar PCB (Guau...)

void 	recibirPCB(int socket, pcb* unPcb);		// Recibe PCB (NO ME DIGAS!!)



// Definiciones

void enviarPCB(int socket, pcb* unPcb){

}

void recibirPCB(int socket, pcb* unPcb){

}

#endif /* PCB_H_ */
