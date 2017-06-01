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

#include "../librerias/controlErrores.h"

// Este serializador lo único que hace es enviar y recibir datos. Estructurar lo que envían/reciben por ahora van a tener que manejarlo ustedes.
// Separé las funciones para enviar tamaños y datos por separado si necesitan alguna en particular.
// Con los enviar/recibir mensaje pueden hacer los dos pasos de una igual.



// Declaraciones



// Funciones de envío

void	enviarTamanio(int socket, size_t tamanioDatos); 						// Para enviar header de un mensaje.

void	enviarDatos(int socket, void* datos, size_t tamanioDatos); 				// Para enviar datos a otro proceso.

void	enviarMensaje(int socket, void* datos, size_t tamanioDatos); 			// Para enviar header y datos a otro proceso.

void	enviarLista(int socket, t_list* lista, size_t tamanioNodo);				// Para enviar listas.



// Funciones de recepción

size_t	recibirTamanio(int socket); 											// Para recibir header de un mensaje.

void 	recibirDatos(int socket, void* receptor, size_t tamanioDatos);			// Para recibir datos de otro proceso.

void	recibirMensaje(int socket, void* receptor);								// Para recibir header y datos a otro proceso.

void	recibirLista(int socket, t_list* receptor, size_t tamanioNodo);			// Para recibir listas.



// Definiciones

void enviarTamanio(int socket, size_t tamanioDatos){
	int verificador;	// Chequea si los envíos se hicieron correctamente.

	verificador = send(socket, &tamanioDatos, sizeof(size_t), 0);
	esErrorConSalida(verificador, "Error al enviar el tamanio de los datos.");
}

void enviarDatos(int socket, void* emisor, size_t tamanioDatos){
	// A datos hay que ponerle un '&' para pasar el puntero de los datos y se envíe tranqui.
	int verificador;	// Chequea si los envíos se hicieron correctamente.

	verificador = send(socket, emisor, tamanioDatos, 0);
	esErrorConSalida(verificador, "Error al enviar datos.");
}

void enviarMensaje(int socket, void* datos, size_t tamanioDatos){
	enviarTamanio(socket, tamanioDatos);
	enviarDatos(socket, datos, tamanioDatos);
}

void enviarLista(int socket, t_list* lista, size_t tamanioNodo){
	// En el for, se hace un send() por cada nodo de la lista.

	int tamanioLista = list_size(lista); 				// Este es el tamaño de lo recibido.
	int tamanioIndice = tamanioLista / tamanioNodo;		// Esto es para poder recorrer la lista serializada como un array y obtener sus nodos.
	int indice;											// Para hacer un for, porque C no me deja inicializar una variable en el prototipo del for... T_T

	void* nodo = malloc(tamanioNodo);					// Para enviar los nodos de la lista.

	enviarTamanio(socket, tamanioLista);				// Enviar el tamaño total de la lista.

	for(indice = 0; indice < tamanioIndice; indice++)	// Recorro la lista como un Array.
	{
		nodo = list_get(lista, indice);					// Obtengo el nodo de la lista
		enviarDatos(socket, nodo, tamanioNodo);			// Lo envío.
	}

	free(nodo);

}

size_t recibirTamanio(int socket){
	int verificador;	// Chequea si los recibos se hicieron correctamente.
	size_t* tamanio; 	// Indica el tamaño total de lo que se va a recibir.

	verificador = recv(socket, tamanio, sizeof(size_t), 0);
	esErrorConSalida(verificador, "Error al recibir tamanio de los datos.");

	return *tamanio;
}

void recibirDatos(int socket, void* receptor, size_t tamanioDatos){
	// A receptor hay que ponerle un '&' para que se almacene lo que se ubique en el buffer de recv.
	int verificador; 	// Chequea si los recibos se hicieron correctamente.

	verificador = recv(socket, receptor, tamanioDatos, 0);
	esErrorConSalida(verificador, "Error al recibir datos.");
}

void recibirMensaje(int socket, void* receptor){
	size_t tamanio = recibirTamanio(socket);
	recibirDatos(socket, receptor, tamanio);
}

void recibirLista(int socket, t_list* receptor, size_t tamanioNodo){
	// En el for, se hace un recv() por cada nodo de la lista.

	int tamanioLista = recibirTamanio(socket); 			// Este es el tamaño de lo recibido.
	int tamanioIndice = tamanioLista / tamanioNodo;		// Esto es para poder recorrer la lista serializada como un array.
	int indice;											// Para hacer un for, porque C no me deja inicializar una variable en el prototipo del for... T_T

	void* nodo = malloc(tamanioNodo);					// Para recibir los nodos de la lista.

	for(indice = 0; indice < tamanioIndice; indice++)	// Recorrer lista como un Array.
	{
		recibirDatos(socket, nodo, tamanioNodo);		// Recibir nodos de la lista.
		list_add(receptor, nodo);						// Añadir a la lista.
	}

	free(nodo);
}

#endif /* SERIALIZADOR_H_ */
