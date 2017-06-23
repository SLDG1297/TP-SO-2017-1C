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

#include <commons/collections/list.h>
#include <commons/string.h>

#include <parser/metadata_program.h>

#include "./serializador.h"



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
	posicionMemoria 	posicionMemoria; 	// Dice Scarcella que poner estructuras anidadas es un mal diseño... Lo cambiaría, pero denme el OK.
} __attribute__((packed))
variableStack;

// Estructura de la lista de variables que hay en el stack.

typedef struct{
	u_int32_t offset;		// Desplazamiento de la instrucción respecto del inicio del programa.
	u_int32_t longitud;		// Tamaño en bytes de la instrucción.
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
	size_t				cantidadInstrucciones;		// Cantidad de instrucciones del ínidce de código.
	lineaUtil*			indiceCodigo;				// Identifica líneas útiles de un programa.
	size_t				bytesEtiquetas;				// Tamaño en bytes índice de etiquetas.
	char*				indiceEtiqueta;				// Identifica llamadas a funciones.
	size_t				cantidadStack;				// Cantidad de contextos del índice de stack.
	t_list*				indiceStack; 				// Ordena valores almacenados en la pila de funciones con sus valores.
	u_int32_t 			exitCode; 					// Motivo por el cual finalizó un programa.

	// Campos adicionales

} __attribute__((packed))
pcb;

// El Process Control Block que maneja el Kernel y tiene toda la información de los procesos que ejecuta la CPU. Su orden en la estructura determina el protocolo para enviar mensajes.



// Declaraciones

size_t	calcularTamanioPCB(pcb* unPcb);									// Es obvio XD

size_t	calcularTamanioStack(t_list* pila, size_t cantidadDeContextos);	// Este también XD

void	serializarPCB(int socket, paquete* envio, pcb* unPcb);			// Enviar PCB (Guau...)

pcb* 	deserializarPCB(int socket);									// Recibe PCB (NO ME DIGAS!!)

void	preprocesador(char* codigo, pcb* unPcb);						// Preprocesador de código para generar el índice de código del PCB



// Definiciones

size_t calcularTamanioPCB(pcb* unPcb){
	size_t tamanio;

	tamanio 	= 	sizeof(u_int32_t)													// PID
				+	sizeof(u_int32_t)													// Program Counter
				+	sizeof(u_int32_t)													// Cantidad de páginas
				+	sizeof(size_t)														// Cantidad de instrucciones
				+	unPcb->cantidadInstrucciones * sizeof(lineaUtil)					// Índice de Eódigo
				+	sizeof(size_t)														// Tamaño etiquetas
				+	unPcb->bytesEtiquetas												// Índice de Etiquetas
				+	sizeof(size_t)														// Cantidad de contextos
				+	calcularTamanioStack(unPcb->indiceStack, unPcb->cantidadStack)		// Índice de Stack
				+	sizeof(u_int32_t);													// Exit Code

	return tamanio;
}

size_t calcularTamanioStack(t_list* pila, size_t cantidadDeContextos){
	size_t tamanio;
	int indice;

	for(indice = 0; indice < cantidadDeContextos; indice++)
	{
		indiceDeStack* contexto = (indiceDeStack*) list_get(pila, indice);

		tamanio +=	contexto->cantidadArgumentos * sizeof(argStack)
				+	contexto->cantidadVariables * sizeof(variableStack)
				+ 	sizeof(u_int32_t)
				+ 	sizeof(posicionMemoria);
	}

	return tamanio;
}

void serializarPCB(int socket, paquete* envio, pcb* unPcb){
	// TODO: Serializar
}

pcb* deserializarPCB(int socket){
	pcb* unPcb;
	// TODO: Serializar
	return unPcb;
}

void preprocesador(char* codigo, pcb* unPcb){
	size_t tamanio;																// Para calcular tamaños de datos serializados.
	t_metadata_program* programa = metadata_desde_literal(codigo);				// Para generar el índice de código a partir de un script.

	// Asignaciones de datos al PCB.

	unPcb->programCounter = programa->instruccion_inicio;						// Asigna el Program Counter con la primer instrucción a ejecutar.

	unPcb->cantidadInstrucciones = programa->instrucciones_size;				// Asigna la cantidad de instrucciones del programa.

	tamanio = unPcb->cantidadInstrucciones * sizeof(lineaUtil);					// Asigna el tamaño en bytes de la lista de instrucciones serializada: BYTES = CANTIDAD * 2 * INT
	unPcb->indiceCodigo = malloc(tamanio);										// Aloca memoria para el ínidce de código.
	memcpy(unPcb->indiceCodigo, programa->instrucciones_serializado, tamanio);	// Copia la lista serializada en el PCB.

	unPcb->bytesEtiquetas = programa->etiquetas_size;							// Asigna la cantidad de etiquetas de instrucciones del programa.
	tamanio = unPcb->bytesEtiquetas + 1;										// Asigna el tamaño en bytes de la lista serializada de etiquetas.
	unPcb->indiceEtiqueta = malloc(tamanio);									// Aloca memoria para el índice de etiquetas.
	memcpy(unPcb->indiceEtiqueta, programa->etiquetas, tamanio);				// Copia la lista serializada en el PCB.

	metadata_destruir(programa);
}

#endif /* PCB_H_ */
