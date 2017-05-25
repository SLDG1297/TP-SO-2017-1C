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

void	enviarTamanio(int socket, size_t tamanioDatos); 				// Para enviar header de un mensaje.

void	enviarDatos(int socket, void* datos, size_t tamanioDatos); 		// Para enviar datos a otro proceso.

void	enviarMensaje(int socket, void* datos, size_t tamanioDatos); 	// Para enviar header y datos a otro proceso.

size_t	recibirTamanio(int socket); 									// Para recibir header de un mensaje.

void*	recibirDatos(int socket, size_t tamanioDatos);					// Para recibir datos de otro proceso.

void*	recibirMensaje(int socket);										// Para recibir header y datos a otro proceso.

t_list* recibirLista(int socket, int tamanioNodo);						// Para recibir listas.

// Definiciones

void enviarTamanio(int socket, size_t tamanioDatos){
	int verificador;	// Chequea si los envíos se hicieron correctamente.

	verificador = send(socket, (size_t*)tamanioDatos, sizeof(size_t), 0);
	esErrorConSalida(verificador, "Error al enviar el tamanio de los datos.");
}

void enviarDatos(int socket, void* datos, size_t tamanioDatos){
	int verificador;	// Chequea si los envíos se hicieron correctamente.

	verificador = send(socket, datos, tamanioDatos, 0);
	esErrorConSalida(verificador, "Error al enviar datos.");
}

void enviarMensaje(int socket, void* datos, size_t tamanioDatos){

	enviarTamanio(socket, tamanioDatos);
	enviarDatos(socket, datos, tamanioDatos);
}

size_t recibirTamanio(int socket){
	int verificador;	// Chequea si los recibos se hicieron correctamente.
	size_t* tamanio; 	// Indica el tamaño total de lo que se va a recibir.

	verificador = recv(socket, tamanio, sizeof(size_t), 0);
	esErrorConSalida(verificador, "Error al recibir tamanio de los datos.");

	return *tamanio;
}

void* recibirDatos(int socket, size_t tamanioDatos){
	int verificador; 						// Chequea si los recibos se hicieron correctamente.
	void* datos  = malloc(tamanioDatos);	// Los datos que se esperan recibir.

	verificador = recv(socket, datos, tamanioDatos, 0);
	esErrorConSalida(verificador, "Error al recibir datos.");

	return datos;
}

void* recibirMensaje(int socket){
	size_t tamanio = recibirTamanio(socket);
	void* datos = recibirDatos(socket, tamanio);

	return datos;
}

t_list* recibirLista(int socket, int tamanioNodo){
	int tamanioLista = recibirTamanio(socket); 			// Este es el tamaño de lo recibido. Lo podría haber sacado del header...
	int tamanioIndice;
	int indice;

	void* listaSerializada = recibirDatos(socket, tamanioLista);

	t_list* lista = NULL;

	tamanioIndice = tamanioLista / tamanioNodo;				// Esto es para poder recorrer la lista serializada como un array.

	for(indice = 0; indice < tamanioIndice; indice++)		// Esto es para crear la lista como se debe.
		// list_add(lista, listaSerializada[indice]);		// Como es un void*, no lo puedo recorrer como array generalmente...

	free(listaSerializada);

	return lista;
}

#endif /* SERIALIZADOR_H_ */
