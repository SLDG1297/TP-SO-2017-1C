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

#include <commons/collections/list.h>

#include "../librerias/controlErrores.h"

// Separé las funciones para enviar tamaños y datos por separado si necesitan alguna en particular.
// Con los enviar/recibir mensaje pueden hacer los dos pasos de una igual.



// Declaraciones



// Funciones para calcular tamaños en bytes comunes (Agreguen alguno de ser necesario)

size_t	tamanioEnBytesChar(char* string);							// Cálculo del tamaño en bytes de char* punteros.

size_t	tamanioEnBytesLista(t_list* lista, size_t tamanioNodo);		// Cálculo del tamaño en bytes de una lista.



// Funciones para serializar (Van a ver que son súper sencillas y medio parásitos, pero es para que sepan lo que están haciendo cuando tengan que enviar estructuras y esas cosas.

void*	crearPaquete(size_t tamanioPaquete);									// Malloc para crear un paquete y enviar una estructura.

void 	destruirPaquete(void* paquete);											// Free del paquete creado.

void 	empaquetar(void* mensaje, void* datos, size_t tamanioDato);				// Para añadir una estructura a un mensaje.

void	desempaquetar(void* datos, void* mensaje, size_t tamanioDato);			// Para obtener una estructura de un mensaje.

void	empaquetarLista(void* mensaje, t_list* lista, size_t tamanioNodos);		// Para serializar listas.

void	desempaquetarLista(t_list* lista, void* mensaje, size_t tamanioNodos);	// Para deserializar listas.

void	referenciarInicio(void* mensaje, size_t tamanioMensaje);				// Para volver a referenciar al inicio de un mensaje.



// Funciones para envío de paquetes serializados

void	enviarDatos(int socket, void* datos, size_t tamanioDatos); 		// Para enviar datos a otro proceso.

void	enviarTamanio(int socket, size_t tamanioDatos); 				// Para enviar header/tamanio de un mensaje/dato.

void	enviarMensaje(int socket, void* datos, size_t tamanioMensaje); 	// Para enviar header y datos a otro proceso.



// Funciones para recepción de paquetes serializados

void 	recibirDatos(int socket, void* datos, size_t tamanioDatos);		// Para recibir datos de otro proceso.

size_t	recibirTamanio(int socket); 									// Para recibir header de un mensaje.

void 	recibirMensaje(int socket, void* datos, size_t tamanioDatos);	// Para recibir header y datos a otro proceso.



// Definiciones

size_t	tamanioEnBytesChar(char* string){
	size_t tamanio = string_length(string);

	return tamanio;
}

size_t	tamanioEnBytesLista(t_list* lista, size_t tamanioNodo){
	size_t tamanio = list_size(lista) * tamanioNodo;

	return tamanio;
}

void* crearPaquete(size_t tamanioPaquete){
	return malloc(tamanioPaquete);
}

void destruirPaquete(void* paquete){
	free(paquete);
}

void empaquetar(void* mensaje, void* datos, size_t tamanioDato){
	memcpy(mensaje, datos, tamanioDato);	// Empaqueta el dato copiándolo en un mensaje.
	mensaje += tamanioDato;					// Desplaza el puntero para poder poner el dato que siga a continuación.
}

void desempaquetar(void* datos, void* mensaje, size_t tamanioDato){
	memcpy(datos, mensaje, tamanioDato);	// Desempaqueta el dato desde un mensaje.
	mensaje += tamanioDato;					// Desplaza el puntero para poder poner el dato que siga a continuación.
}

void empaquetarLista(void* mensaje, t_list* lista, size_t tamanioNodos){
	size_t tamanioLista = tamanioEnBytesLista(lista, tamanioNodos);		// Tamanio de la lista en bytes.
	size_t indiceLista = list_size(lista); 								// Tamanio de la lista para recorrer todos sus elementos.
	int indice;															// Para hacer un for, porque C no me deja inicializar una variable en el prototipo del for... T_T

	empaquetar(mensaje, &tamanioLista, sizeof(size_t));					// Para empaquetar el tamaño de la lista y luego se pueda recorrer como un array.

	for(indice = 0; indice < indiceLista; indice++)						// Recorro la lista como un Array.
	{
		empaquetar(mensaje, list_get(lista, indice), tamanioNodos);		// Serializo un nodo de la lista.
	}

}

void desempaquetarLista(t_list* lista, void* mensaje, size_t tamanioNodos){
	size_t tamanioLista;					// Tamanio de la lista en bytes.
	size_t indiceLista;						// Cantidad de elementos en la lista.
	int indice;								// Para recorrer la lista serializada como un array.
	void* nodo = malloc(tamanioNodos);		// Nodo auxiliar para almacenar el nodo serializado.

	desempaquetar(&tamanioLista, mensaje, sizeof(size_t));	// Obtengo el tamaño en bytes de la lista.
	indiceLista = tamanioLista / tamanioNodos;				// Calculo la cantidad de nodos de la lista con su tamaño en bytes.

	for(indice = 0; indice < indiceLista; indice++)
	{
		desempaquetar(nodo, mensaje, tamanioNodos);
		list_add(lista, nodo);
	}

	free(nodo);
}

void referenciarInicio(void* mensaje, size_t tamanioMensaje){
	mensaje -= tamanioMensaje;	// Con esto, el puntero vuelve a referenciar al inicio del mensaje.
}

void enviarDatos(int socket, void* datos, size_t tamanioDatos){
	// A datos hay que ponerle un '&' para pasar el puntero de los datos y se envíe tranqui.
	int verificador;											// Chequea si los envíos se hicieron correctamente.

	verificador = send(socket, datos, tamanioDatos, 0);			// Enviar datos serializados.
	esErrorConSalida(verificador, "Error al enviar datos.");
}

void enviarTamanio(int socket, size_t tamanio){
	enviarDatos(socket, &tamanio, sizeof(size_t));
}

void enviarMensaje(int socket, void* mensaje, size_t tamanioMensaje){
	size_t tamanioEnviado = sizeof(size_t) + tamanioMensaje;			// Cantidad de bytes enviados en el mensaje = Header + Tamanio Mensaje.
	void* mensajeCompleto = crearPaquete(tamanioEnviado);				// Aloca memoria para enviar el mensaje.

	referenciarInicio(mensaje, tamanioMensaje);							// Para que el puntero referencia al comienzo de lo que van a enviar y no lo tengan que hacer cada vez que empaqueten datos.

	empaquetar(mensajeCompleto, &tamanioMensaje, sizeof(size_t));		// Copia en mensaje el tamaño de lo que se va a enviar.
	empaquetar(mensajeCompleto, mensaje, tamanioMensaje);				// Copia los datos a enviar en el mensaje.

	referenciarInicio(mensajeCompleto, tamanioEnviado);					// Para que el puntero referencie al comienzo del mensaje y se envíe correctamente.

	enviarDatos(socket, mensajeCompleto, tamanioEnviado);				// Enviar mensaje serializado.

	destruirPaquete(mensajeCompleto);
}

void recibirDatos(int socket, void* datos, size_t tamanioDatos){
	// A receptor hay que ponerle un '&' para que se almacene lo que se ubique en el buffer de recv.
	int verificador; 	// Chequea si los recibos se hicieron correctamente.

	verificador = recv(socket, datos, tamanioDatos, 0);
	esErrorConSalida(verificador, "Error al recibir datos.");
}

size_t recibirTamanio(int socket){
	size_t tamanio;
	recibirDatos(socket, &tamanio, sizeof(size_t));

	return tamanio;
}

void recibirMensaje(int socket, void* datos, size_t tamanioDatos){
	size_t tamanioRecibido;
	void* mensaje;

	recibirDatos(socket, &tamanioRecibido, sizeof(u_int32_t));

	mensaje = malloc(tamanioRecibido);
	recibirDatos(socket, mensaje, tamanioRecibido);

	desempaquetar(datos, mensaje, tamanioDatos);

	free(mensaje);
}

#endif /* SERIALIZADOR_H_ */
