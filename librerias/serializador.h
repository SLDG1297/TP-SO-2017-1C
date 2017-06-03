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

// Separé las funciones para enviar tamaños y datos por separado si necesitan alguna en particular.
// Con los enviar/recibir mensaje pueden hacer los dos pasos de una igual.



// Declaraciones



// Funciones de envío

void	enviarTamanio(int socket, size_t tamanioDatos); 				// Para enviar header de un mensaje.

void	enviarDatos(int socket, void* datos, size_t tamanioDatos); 		// Para enviar datos a otro proceso.

void	enviarMensaje(int socket, void* datos, size_t tamanioDatos); 	// Para enviar header y datos a otro proceso.

void	enviarLista(int socket, t_list* lista, size_t tamanioNodos);	// Para enviar listas.



// Funciones de recepción

size_t	recibirTamanio(int socket); 									// Para recibir header de un mensaje.

void 	recibirDatos(int socket, void* datos, size_t tamanioDatos);		// Para recibir datos de otro proceso.

void	recibirMensaje(int socket, void* datos);						// Para recibir header y datos a otro proceso.

void	recibirLista(int socket, t_list* lista, size_t tamanioNodos);	// Para recibir listas.



// Definiciones

void enviarTamanio(int socket, size_t tamanioDatos){
	int verificador;								// Chequea si los envíos se hicieron correctamente.
	void* header = malloc(sizeof(size_t));			// Aloca memoria contigua para enviar el mensaje correctamente.

	memcpy(header, &tamanioDatos, sizeof(size_t));	// Copia en memoria los bytes para ser enviados correctamente.

	verificador = send(socket, header, sizeof(size_t), 0);
	esErrorConSalida(verificador, "Error al enviar el tamanio de los datos.");	// Enviar tamaño de datos serializados.

	free(header);
}

void enviarDatos(int socket, void* datos, size_t tamanioDatos){
	// A datos hay que ponerle un '&' para pasar el puntero de los datos y se envíe tranqui.
	int verificador;							// Chequea si los envíos se hicieron correctamente.
	void* mensaje = malloc(tamanioDatos);		// Aloca memoria contigua para enviar el mensaje correctamente.

	memcpy(mensaje, datos, tamanioDatos);		// Copia en memoria los bytes para ser enviados correctamente.

	verificador = send(socket, mensaje, tamanioDatos, 0);		// Enviar datos serializados.
	esErrorConSalida(verificador, "Error al enviar datos.");

	free(mensaje);
}

void enviarMensaje(int socket, void* datos, size_t tamanioDatos){
	enviarTamanio(socket, tamanioDatos);
	enviarDatos(socket, datos, tamanioDatos);
}

void enviarLista(int socket, t_list* lista, size_t tamanioNodos){
	// En el for, se hace un send() por cada nodo de la lista.
	size_t tamanioLista = list_size(lista); 			// Tamanio de la lista para recorrer todos sus elementos.
	int indice;											// Para hacer un for, porque C no me deja inicializar una variable en el prototipo del for... T_T

	enviarTamanio(socket, tamanioLista);				// Para que el receptor sepa la cantidad de elementos que tiene la lista.

	for(indice = 0; indice < tamanioLista; indice++)	// Recorro la lista como un Array.
	{
		void* nodo = malloc(tamanioNodos);
		nodo = list_get(lista, indice);					// Obtengo el nodo de la lista.
		enviarDatos(socket, nodo, tamanioNodos);		// Lo envío.
		free(nodo);
	}

}

size_t recibirTamanio(int socket){
	int verificador;							// Chequea si los recibos se hicieron correctamente.
	size_t tamanio;								// Indica el tamaño total de lo que se va a recibir.
	void* receptor = malloc(sizeof(size_t)); 	// Para recibir el tamaño.

	verificador = recv(socket, receptor, sizeof(size_t), 0);
	esErrorConSalida(verificador, "Error al recibir tamanio de los datos.");

	memcpy(&tamanio, receptor, sizeof(size_t));

	free(receptor);

	return tamanio;
}

void recibirDatos(int socket, void* datos, size_t tamanioDatos){
	// A receptor hay que ponerle un '&' para que se almacene lo que se ubique en el buffer de recv.
	int verificador; 							// Chequea si los recibos se hicieron correctamente.
	void* receptor = malloc(tamanioDatos);		// Aloca memoria contigua para recibir el mensaje correctamente.

	verificador = recv(socket, receptor, tamanioDatos, 0);
	esErrorConSalida(verificador, "Error al recibir datos.");

	memcpy(datos, receptor, tamanioDatos);		// Asigna lo recibido a la referencia en la cual se quiere recibir los datos.

	free(receptor);
}

void recibirMensaje(int socket, void* datos){
	size_t tamanio = recibirTamanio(socket);
	recibirDatos(socket, datos, tamanio);
}

void recibirLista(int socket, t_list* lista, size_t tamanioNodos){
	// En el for, se hace un recv() por cada nodo de la lista.
	int tamanioIndice = recibirTamanio(socket);			// Esto es para poder recorrer la lista serializada como un array.
	int indice;											// Para hacer un for, porque C no me deja inicializar una variable en el prototipo del for... T_T

	for(indice = 0; indice < tamanioIndice; indice++)	// Recorrer lista como un Array.
	{
		void* nodo = malloc(tamanioNodos);				// Aloca memoria para recibir un nodo correctamente.
		recibirDatos(socket, nodo, tamanioNodos);		// Recibir nodos de la lista.
		list_add(lista, nodo);							// Añadir a la lista.
		free(nodo);										// Libera memoria para que no se leakee.
	}
}

#endif /* SERIALIZADOR_H_ */
