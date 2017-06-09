/*
 * serializador.h
 *
 *  Created on: 9/5/2017
 *      Author: utnso
 */

#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <commons/string.h>
#include <commons/collections/list.h>

#include "../librerias/controlErrores.h"



// Estructuras de datos

typedef void** paquete;

// Una estructura donde se copian bytes para enviar por socket.

// Menos pregunta a Dios y perdona...

// TODO: IMPORTANTE: Esta estructura no funciona bien... Sigan empaquetando con void* paquete = malloc(suTamanio).
// Leer el choclo de abajo por si te interesa saber qué es lo que debería abstraer.

// Para empaquetar, tengo que correr la dirección del puntero para que no se me pisen las cosas.
// Pero necesito hacer una asignación destructiva del puntero adentro de la función empaquetar para hacer la interfaz más sencilla.
// Entonces tengo que pasar la dirección de un puntero a la función.
// Y es un puntero de tipo void*, por lo que para dereferenciar eso, tengo que hacer (void*) *
// Vendría a ser una especie de (int*) *;
// Cuando quieran les explico mejor XD



// Declaraciones


// Funciones para enviar y recibir datos (abstracciones de send y recv)

void	enviarDato(int socket, void* dato, size_t tamanioDato);					// Para enviar un dato con un send.

void	recibirDato(int socket, void* datos, size_t tamanioDato);				// Para recibir un dato de un recv.



// Funciones para serializar

paquete crearPaquete(size_t tamanioPaquete);									// Malloc para crear un paquete y enviar una estructura.

void 	destruirPaquete(paquete unPaquete);										// Free del paquete creado.

void 	empaquetar(paquete envio, void* datos, size_t tamanioDato);				// Para añadir una estructura a un mensaje.

void	empaquetarVariable(paquete unPaquete, void* datos, size_t tamanioDato);	// Para añadir una estructura de tamaño variable, como un char*.

void 	empaquetarLista(paquete unPaquete, t_list* lista, size_t tamanioNodo);	// Para serializar listas con nodos de tamaño fijo.

void	empaquetarListaVariable(paquete unPaquete, t_list* lista);				// TODO: Para empaquetar listas con nodos de tamaño variables, como si tuvieran un char* en su nodo.

void 	enviarPaquete(int socket, paquete unPaquete, size_t tamanioEnvio);		// Send del paquete armado.



// Funciones para deserializar

void 	recibirPaquete(int socket, void* receptor, size_t tamanioRecibo);		// Recv del paquete que llega por socket.

void	recibirPaqueteVariable(int socket, void** receptor);					// Recv de un paquete con tamño de datos variable.

void	recibirLista(int socket, t_list* lista, size_t tamanioNodo);			// Para deserializar listas.

void	recibirListaVariable(int socket, t_list* lista);						// TODO: Para deserializar listas con char* y esas cosas turbias.



// Funciones de envío/recibo que me pidieron que deje.

void	enviarTamanio(int socket, size_t tamanioDatos); 						// Para enviar tamaño de un dato.

size_t	recibirTamanio(int socket); 											// Para recibir tamaño de un mensaje.



// Funciones para calcular tamaños en bytes comunes (Agreguen alguno de ser necesario)

size_t 	tamanioEnBytesVariables(u_int32_t cantidadDatosVariables);				// Tamaño extra producido por necesitar enviar un dato de tamaño variable.

size_t	tamanioEnBytesString(char* string);										// Cálculo del tamaño en bytes de char* punteros.

size_t	tamanioEnBytesListaFija(t_list* lista, size_t tamanioNodo);				// Cálculo del tamaño en bytes de una lista con nodos de tamaño fijo.



// Definiciones



void enviarDato(int socket, void* dato, size_t tamanioDato){
	int verificador;													// Chequea que el envío sea correcto.

	verificador = send(socket, dato, tamanioDato, 0);					// Envía el dato.
	esErrorConSalida(verificador, "\nNo se pudo enviar el dato.\n");
}

void recibirDato(int socket, void* datos, size_t tamanioDato){
	int verificador;													// Chequea que el recibo sea correcto.

	verificador = recv(socket, datos, tamanioDato, 0);					// Recibe el dato.
	esErrorConSalida(verificador, "\nNo se pudo recibir el paquete.\n");
}

paquete crearPaquete(size_t tamanio){
	paquete unPaquete;
	*unPaquete = malloc(tamanio);

	return unPaquete;
}

void destruirPaquete(paquete unPaquete){
	free(*unPaquete);
}

void empaquetar(paquete unPaquete, void* datos, size_t tamanioDato){
	memcpy(*unPaquete, datos, tamanioDato);		// Empaqueta el dato copiándolo en un mensaje.
	*unPaquete += tamanioDato;					// Desplaza el puntero para poder poner el dato que siga a continuación.
}

void empaquetarVariable(paquete unPaquete, void* datos, size_t tamanioDato){
	empaquetar(unPaquete, &tamanioDato, sizeof(size_t));

	empaquetar(unPaquete, datos, tamanioDato);
}

void empaquetarLista(paquete unPaquete, t_list* lista, size_t tamanioNodo){
	u_int32_t indice;													// Para recorrer la lista como un array.
	u_int32_t cantidadElementos = list_size(lista);						// Para saber cuántos elementos enviar.

	empaquetar(unPaquete, &cantidadElementos, sizeof(u_int32_t));		// Para que el receptor sepa cuántos nodos va a tener la lista.

	for(indice = 0; indice < cantidadElementos; indice++)
	{
		empaquetar(unPaquete, list_get(lista, indice), tamanioNodo);	// Lo empaqueto para el send().
	}
}

void enviarPaquete(int socket, paquete paqueteEnvio, size_t tamanioEnvio){
	int verificador;														// Para verificar que el envío fue correcto.
	*paqueteEnvio -= tamanioEnvio;											// Reubica el puntero del paquete para referenciar a inicio y enviarlo correctamente.

	enviarDato(socket, *paqueteEnvio, tamanioEnvio);

	esErrorConSalida(verificador, "\nNo se pudo enviar el paquete.\n");

	destruirPaquete(paqueteEnvio);											// Libera el paquete.
}

void recibirPaquete(int socket, void* receptor, size_t tamanioRecibo){
	recibirDato(socket, receptor, tamanioRecibo);	// Re parásito XD
}

void recibirPaqueteVariable(int socket, void** receptor){
	size_t tamanio;

	recibirDato(socket, &tamanio, sizeof(size_t));

	*receptor = malloc(tamanio);

	recibirDato(socket, *receptor, tamanio);
}

void recibirLista(int socket, t_list* lista, size_t tamanioNodo){
	u_int32_t indice;				// Para agregar nodos a la lista como un array.
	u_int32_t cantidadElementos;	// Para saber el tope de la lista y cuando parar de añadir objetos a la misma.

	recibirPaquete(socket, &cantidadElementos, sizeof(size_t));	// Recibe el tamaño de la lista.

	for(indice = 0; indice < cantidadElementos; indice++)
	{
		void* nodo = malloc(tamanioNodo);			// Aloco memoria para un nodo.

		recibirPaquete(socket, nodo, tamanioNodo);	// Recibo el nodo.
		list_add(lista, nodo);						// Lo añado a la lista.
	}
}

void enviarTamanio(int socket, size_t tamanio){
	enviarDato(socket, &tamanio, sizeof(size_t));
}

size_t recibirTamanio(int socket){
	size_t tamanio;
	recibirDato(socket, &tamanio, sizeof(size_t));

	return tamanio;
}

size_t tamanioEnBytesVariables(u_int32_t cantidadDatosVariables){
	return cantidadDatosVariables * sizeof(size_t);
}

size_t tamanioEnBytesString(char* string){
	size_t tamanio = string_length(string) + 1;

	return tamanio;
}

size_t tamanioEnBytesListaFija(t_list* lista, size_t tamanioNodo){
	size_t tamanio = list_size(lista) * tamanioNodo;

	return tamanio;
}

#endif /* SERIALIZADOR_H_ */
