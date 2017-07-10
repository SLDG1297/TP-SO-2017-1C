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

pcb 		crearPCB(u_int32_t pid, char* path, u_int32_t tamanioPaginas);		// Crea un PCB...

char* 		obtenerCodigo(char* path);											// Obtiene el código de un programa a partir de un script

void		preprocesador(char* codigo, pcb* unPcb);							// Generar el índice de código del PCB

size_t		calcularTamanioPCB(pcb* unPcb);										// Es obvio XD

size_t		calcularTamanioStack(t_list* pila);									// Este también XD

paquete*	serializarPCB(int socket, pcb* unPcb);								// Serializar PCB (Guau...)

pcb* 		deserializarPCB(int socket);										// Deserializar PCB (NO ME DIGAS!!)


// Definiciones

pcb crearPCB(u_int32_t pid, char* path, u_int32_t tamanioPaginas){
	pcb nuevoProceso;							// Inicializar PCB

	nuevoProceso.pid = pid;						// Asignar PID

	char* codigo = obtenerCodigo(path);			// El File System devuelve el código de un archivo.

	preprocesador(codigo, &nuevoProceso);		// Obtengo los índices de etiquetas y de código del código.

	nuevoProceso.paginasUsadas = nuevoProceso.cantidadInstrucciones / tamanioPaginas;	// Asignar páginas usadas por el código.

	nuevoProceso.indiceStack = list_create();	// Inicializar stack;

	free(codigo);

	return nuevoProceso;
}

char* obtenerCodigo(char* path){
	char* codigo;
	long int tamanio;

	FILE* archivo = fopen(path, "r");

	fseek(archivo, 0, SEEK_END);
	tamanio = ftell(archivo);
	rewind(archivo);

	codigo = malloc(tamanio);
	fread(codigo, sizeof(char), tamanio, archivo);

	fclose(archivo);

	return codigo;
}

size_t calcularTamanioPCB(pcb* unPcb){
	size_t tamanio;

	tamanio 	= 	sizeof(u_int32_t)													// PID
				+	sizeof(u_int32_t)													// Program Counter
				+	sizeof(u_int32_t)													// Cantidad de páginas
				+	sizeof(size_t)														// Cantidad de instrucciones
				+	unPcb->cantidadInstrucciones * sizeof(lineaUtil)					// Índice de Código
				+	sizeof(size_t)														// Tamaño etiquetas
				+	unPcb->bytesEtiquetas												// Índice de Etiquetas
				+	sizeof(size_t)														// Cantidad de contextos
				+	calcularTamanioStack(unPcb->indiceStack)							// Índice de Stack
				+	sizeof(u_int32_t);													// Exit Code

	return tamanio;
}

size_t calcularTamanioStack(t_list* pila){
	size_t tamanio;
	size_t cantidadDeContextos = list_size(pila);

	int indice = 0;
	while(indice < cantidadDeContextos)
	{
		indiceDeStack* contexto = (indiceDeStack*) list_get(pila, indice);

		tamanio +=	contexto->cantidadArgumentos * sizeof(argStack)						// Tamaño de la lista de argumentos
				+	sizeof(u_int32_t)													// Para empaquetar índice de la lista de argumentos
				+	contexto->cantidadVariables * sizeof(variableStack)					// Tamaño de la lista de variables
				+	sizeof(u_int32_t)													// Para empaquetar índice de la lista de variables
				+ 	sizeof(u_int32_t)													// Tamaño del program counter que se devuelve cuando finaliza la función
				+ 	sizeof(posicionMemoria);											// Tamaño de la posició de retorno

		indice++;
	}

	return tamanio;
}

paquete* serializarPCB(int socket, pcb* unPcb){
	// TODO: Testear Serialización PCB
	paquete* envio = crearPaquete(calcularTamanioPCB(unPcb));

	empaquetar(envio, &unPcb->pid, sizeof(u_int32_t));

	empaquetar(envio, &unPcb->programCounter, sizeof(u_int32_t));

	empaquetar(envio, &unPcb->paginasUsadas, sizeof(u_int32_t));

	empaquetarVariable(envio, &unPcb->indiceCodigo, unPcb->cantidadInstrucciones * sizeof(lineaUtil));

	empaquetarVariable(envio, &unPcb->indiceEtiqueta, unPcb->bytesEtiquetas);

	empaquetar(envio, &unPcb->cantidadStack, sizeof(size_t));

	int i = 0;
	while(i < list_size(unPcb->indiceStack))
	{
		indiceDeStack* contexto = (indiceDeStack *) list_get(unPcb->indiceStack, i);

		empaquetar(envio, &contexto->cantidadArgumentos, sizeof(u_int32_t));
		empaquetarLista(envio, contexto->argumentos, sizeof(argStack));

		empaquetar(envio, &contexto->cantidadVariables, sizeof(u_int32_t));
		empaquetarLista(envio, contexto->variables, sizeof(variableStack));

		empaquetar(envio, &contexto->retPos, sizeof(u_int32_t));
		empaquetar(envio, &contexto->retVar, sizeof(posicionMemoria));

		i++;
	};

	empaquetar(envio, &unPcb->exitCode, sizeof(u_int32_t));

	return envio;
}

pcb* deserializarPCB(int socket){
	// TODO: Testear Deserialización PCB
	pcb* unPcb;

	recibirPaquete(socket, &unPcb->pid, sizeof(u_int32_t));

	recibirPaquete(socket, &unPcb->programCounter, sizeof(u_int32_t));

	recibirPaquete(socket, &unPcb->paginasUsadas, sizeof(u_int32_t));

	unPcb->cantidadInstrucciones = recibirPaqueteVariable(socket, (void**)&unPcb->indiceCodigo);	// Jaja DDS. Mirá como tiro algo paso por parámetro algo mutable y devuelvo un valor XDXDXD

	unPcb->bytesEtiquetas = recibirPaqueteVariable(socket, (void**)&unPcb->indiceEtiqueta);			// Chiteadísimo...

	recibirPaquete(socket, &unPcb->cantidadStack, sizeof(size_t));

	// TODO: Recibir Stack

	recibirPaquete(socket, &unPcb->exitCode, sizeof(u_int32_t));

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

	tamanio = unPcb->bytesEtiquetas;											// Asigna el tamaño en bytes de la lista serializada de etiquetas.
	unPcb->indiceEtiqueta = malloc(tamanio);									// Aloca memoria para el índice de etiquetas.
	memcpy(unPcb->indiceEtiqueta, programa->etiquetas, tamanio);				// Asigna el índice de etiquetas en el PCB.

	metadata_destruir(programa);
}

#endif /* PCB_H_ */
