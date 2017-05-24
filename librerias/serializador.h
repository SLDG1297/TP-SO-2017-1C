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

void	enviarDatos(int socket, void* datos, size_t tamanioDatos); 	// Para enviar datos a otro proceso.

void*	recibirDatos(int socket); 								 	// Para recibir datos de otro proceso.

void enviarDatos(int socket, void* datos, size_t tamanioDatos)
{
	int verificador; 									// Chequea si los envíos se hicieron correctamente.
	void* tamanioAux = malloc(sizeof(tamanioDatos)); 	// Para que no hinche el send...
	tamanioAux = &tamanioDatos;

	// Indica el tamaño de lo que se envía a continuación.
	verificador = send(socket, tamanioAux, sizeof(int), 0);
	free(tamanioAux);
	esErrorConSalida(verificador, "Error al enviar el tamanio de los datos.");

	// Envío de los datos con el tamanio adecuado. (Payload)
	verificador = send(socket, datos, tamanioDatos, 0);
	esErrorConSalida(verificador, "Error al enviar datos.");
}

void* recibirDatos(int socket)
{
	int verificador; 							// Chequea si los recibos se hicieron correctamente.
	size_t tamanio; 							// Indica el tamaño total de lo que se va a recibir.
	void* tamanioAux = malloc(sizeof(size_t)); 	// Para que recv no hinche...
	void* datos;								// Los datos que se esperan recibir.

	// Recibir el tamaño total de los datos recibidos.
	verificador = recv(socket, tamanioAux, sizeof(size_t), 0);
	esErrorConSalida(verificador, "Error al recibir tamanio de los datos.");
	tamanio = *((size_t*)tamanioAux);
	free(tamanioAux);

	// Recibir los datos con el tamanio adecuado.
	datos = malloc(tamanio);
	verificador = recv(socket, datos, tamanio, 0);
	esErrorConSalida(verificador, "Error al recibir datos.");

	return datos;
}

t_list* recibirLista(int socket, int tamanioNodo, void* listaSerializada)
{
	int tamanioRecibido, tamanioIndice;
	t_list* lista;

	tamanioRecibido = sizeof(*listaSerializada);		// Este es el tamaño de lo recibido. Lo podría haber sacado del header...
	tamanioIndice = tamanioRecibido / tamanioNodo;		// Esto es para poder recorrer la lista serializada como un array.

	for(int i = 0; i < tamanioIndice; i++)				// Esto es para crear la lista como se debe.
		list_add(lista, listaSerializada[i]);

	free(listaSerializada);

	return lista;
}

#endif /* SERIALIZADOR_H_ */
