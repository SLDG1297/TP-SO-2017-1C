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
	u_int32_t	primerInstruccion;		// El numero de la primera instrucción (Begin).
	size_t		cantidadInstrucciones;	// Cantidad de instrucciones del programa.
	lineaUtil*	instrucciones; 			// Instrucciones del programa.
} __attribute__((packed))
indiceDeCodigo;

// El índice de Código del PCB con las líneas útiles de código.

typedef struct{
	size_t	cantidadEtiquetas;	// Cantidad total de etiquetas.
	char*	etiquetas;			// La serializacion de las etiquetas.
} __attribute__((packed))
indiceDeEtiquetas;

// El índice de Etiqueta del PCB para poder identificar las funciones de un programa.

typedef struct{
	t_list*				argumentos; 	// Posiciones de memoria donde se almacenan las copias de los argumentos de la función.
	t_list*				variables; 		// Identificadores y posiciones de memoria donde se almacenan las variables locales de la función.
	u_int32_t			retPos; 		// Posición del índice de código donde se debe retornar al finalizar la ejecución de la función.
	posicionMemoria		retVar; 		// Posición de memoria donde se debe almacenar el resultado de la función provisto por la sentencia RETURN.
} __attribute__((packed))
indiceDeStack;

// El índice de Stack del PCB para poder hacer llamadas a procedimientos con sus argumentos.

typedef struct{
	u_int32_t 			pid; 				// Identificador de un proceso.
	u_int32_t 			programCounter; 	// Program counter: indica el número de la siguiente instrucción a ejecutarse.
	u_int32_t 			paginasUsadas; 		// Cantidad de páginas usadas por el programa (Desde 0).
	indiceDeCodigo		indiceCodigo;		// Identifica líneas útiles de un programa.
	indiceDeEtiquetas	indiceEtiqueta;		// Identifica llamadas a funciones.
	t_list*				indiceStack; 		// Ordena valores almacenados en la pila de funciones con sus valores.
	u_int32_t 			exitCode; 			// Motivo por el cual finalizó un programa.
} __attribute__((packed))
pcb;

// El Process Control Block que maneja el Kernel y tiene toda la información de los procesos que ejecuta la CPU.



// Declaraciones

void	enviarPCB(int socket, pcb* unPcb);						// Enviar PCB (Guau...)

void 	recibirPCB(int socket, pcb* unPcb);						// Recibe PCB (NO ME DIGAS!!)

void	enviarStack(int socket, t_list* indiceStack);			// Envía el contexto local de un proceso.

t_list* recibirStack(int socket);								// Recibe el contexto local de un proceso.



// Definiciones

void enviarPCB(int socket, pcb* unPcb){
	// Creo que es bastante claro, no es necesario que explique cada línea, o sí? Explico las que sean turbias.

	size_t tamanioEnvio; // Esto es para asignar el valor de lo enviado a una variable.

	enviarDatos(socket, &unPcb->pid, sizeof(u_int32_t));

	enviarDatos(socket, &unPcb->programCounter, sizeof(u_int32_t));

	enviarDatos(socket, &unPcb->paginasUsadas, sizeof(u_int32_t));

	enviarDatos(socket, &unPcb->indiceCodigo.primerInstruccion, sizeof(u_int32_t));

	enviarDatos(socket, &unPcb->indiceCodigo.cantidadInstrucciones, sizeof(size_t));

	tamanioEnvio = (unPcb->indiceCodigo.cantidadInstrucciones) * sizeof(lineaUtil);		// Cálculo = cantidad de instrucciones * 2 * u_int_32

	enviarMensaje(socket, &unPcb->indiceCodigo.instrucciones, tamanioEnvio);

	enviarDatos(socket, &unPcb->indiceEtiqueta.cantidadEtiquetas, sizeof(size_t));

	tamanioEnvio = sizeof(char) * strlen(unPcb->indiceEtiqueta.etiquetas) + 1;			// El + 1 es porque todos los Strings terminan con '\0' y es necesario que esté.

	enviarMensaje(socket, &unPcb->indiceEtiqueta.etiquetas, tamanioEnvio);

	enviarStack(socket, unPcb->indiceStack);

	enviarDatos(socket, &unPcb->exitCode, sizeof(u_int32_t));

}

void recibirPCB(int socket, pcb* unPcb){
	// Bendita abstracción de código. Esto es copypaste del bueno.

	recibirDatos(socket, &unPcb->pid, sizeof(u_int32_t));

	recibirDatos(socket, &unPcb->programCounter, sizeof(u_int32_t));

	recibirDatos(socket, &unPcb->paginasUsadas, sizeof(u_int32_t));

	recibirDatos(socket, &unPcb->indiceCodigo.primerInstruccion, sizeof(u_int32_t));

	recibirDatos(socket, &unPcb->indiceCodigo.cantidadInstrucciones, sizeof(size_t));

	recibirMensaje(socket, &unPcb->indiceCodigo.instrucciones);

	recibirDatos(socket, &unPcb->indiceEtiqueta.cantidadEtiquetas, sizeof(size_t));

	recibirMensaje(socket, &unPcb->indiceEtiqueta.etiquetas);

	unPcb->indiceStack = recibirStack(socket);

	recibirDatos(socket, &unPcb->exitCode, sizeof(u_int32_t));

}

void enviarStack(int socket, t_list* pila){
	size_t tamanioStack = list_size(pila);	// Esto es el tamaño del indice de Stack. Para recorrerlo como un array.
	indiceDeStack contexto;					// Esto es un nodo del indice de Stack.
	int indice;								// Para recorrer la lista con un for.

	enviarTamanio(socket, tamanioStack);	// Envía el tamaño de la lista al receptor para que sepa cuántos nodos va a recibir.

	for(indice = 0; indice < tamanioStack; indice++)						// Envío del stack.
	{
		contexto = *(indiceDeStack*)list_get(pila, indice);					// Obtener nodo i-ésimo del stack. Lo que sigue es enviarlo.
		enviarLista(socket, contexto.argumentos, sizeof(argStack));
		enviarLista(socket, contexto.variables, sizeof(variableStack));
		enviarMensaje(socket, &contexto.retPos, sizeof(u_int32_t));
		enviarMensaje(socket, &contexto.retVar, sizeof(posicionMemoria));
	}
}

t_list* recibirStack(int socket){
	t_list* pila = list_create();					// Inicializar el índice de Stack.
	indiceDeStack contexto;							// Nodo del Stack que se recibe.
	int indice;										// Para recorrer la lista con un for.

	size_t tamanioStack = recibirTamanio(socket);	// El tamaño que va a tener la lista.

	for(indice = 0; indice < tamanioStack; indice++)
	{
		recibirLista(socket, contexto.argumentos, sizeof(argStack));
		recibirLista(socket, contexto.variables, sizeof(variableStack));
		recibirMensaje(socket, &contexto.retPos);
		recibirMensaje(socket, &contexto.retVar);
		list_add(pila, &contexto);	// Añade un contexto al índice de Stack.
	}

	return pila;
}

#endif /* PCB_H_ */
