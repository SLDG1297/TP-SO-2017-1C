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

typedef struct{
	size_t tamanio;
	size_t tamanioUsado;
	void* contenido;
} paquete;

// Una estructura donde se copian bytes para enviar por socket.



// Declaraciones


// Funciones para enviar y recibir datos (abstracciones de send y recv)

void		enviarDato(int socket, void* dato, size_t tamanioDato);					// Para enviar un dato con un send.

void		recibirDato(int socket, void* dato, size_t tamanioDato);				// Para recibir un dato de un recv.



// Funciones para serializar

paquete*	crearPaquete(size_t tamanio);														// Malloc para crear un paquete y enviar una estructura.

paquete*	crearHeader(size_t tamanio, u_int32_t servicio);									// Crea un paquete para solicitar un servicio definido en el protocolo.

void 		destruirPaquete(paquete* envio);													// Free del paquete creado.

void 		empaquetar(paquete* envio, void* datos, size_t tamanioDato);						// Para añadir una estructura a un mensaje.

void		empaquetarVariable(paquete* envio, void* datos, size_t tamanioDato);				// Para añadir una estructura de tamaño variable, como un char*.

void 		empaquetarLista(paquete* envio, t_list* lista, size_t tamanioNodo);					// Para serializar listas con nodos de tamaño fijo.

void 		enviarPaquete(int socket, paquete* envio);											// Send del paquete armado.



// Funciones para deserializar

void 		recibirPaquete(int socket, void* receptor, size_t tamanioRecibo);					// Recv del paquete que llega por socket.

size_t		recibirPaqueteVariable(int socket, void** receptor);								// Recv de un paquete con tamaño de datos variable. Devuelve el tamaño del dato.

size_t		recibirLista(int socket, t_list* lista, size_t tamanioNodo);						// Para deserializar listas. Devuelve el tamaño de la lista.

u_int32_t	recibirHeader(int socket);															// Nada loco. Recibe la acción a realizar. Hecho por un tema de sencillez.


// Funciones de envío/recibo que me pidieron que deje.

void		enviarTamanio(int socket, size_t tamanioDatos); 									// Para enviar tamaño de un dato.

size_t		recibirTamanio(int socket); 														// Para recibir tamaño de un mensaje.



// Funciones para calcular tamaños en bytes comunes (Agreguen alguno de ser necesario)

size_t 		tamanioEnBytesVariables(u_int32_t cantidadDatosVariables);							// Tamaño extra producido por necesitar enviar un dato de tamaño variable.

size_t		tamanioEnBytesString(char* string);													// Cálculo del tamaño en bytes de char* punteros.

size_t		tamanioEnBytesListaFija(t_list* lista, size_t tamanioNodo);							// Cálculo del tamaño en bytes de una lista con nodos de tamaño fijo.



// Definiciones



void enviarDato(int socket, void* dato, size_t tamanioDato){
	int verificador;													// Chequea que el envío sea correcto.

	verificador = send(socket, dato, tamanioDato, 0);					// Envía el dato.
	esErrorConSalida(verificador, "\nNo se pudo enviar el dato.\n");
}

void recibirDato(int socket, void* dato, size_t tamanioDato){
	int verificador;													// Chequea que el recibo sea correcto.

	verificador = recv(socket, dato, tamanioDato, 0);					// Recibe el dato.
	esErrorConSalida(verificador, "\nNo se pudo recibir el paquete.\n");
}

paquete* crearPaquete(size_t tamanio){
	paquete* envio = malloc(2 * sizeof(size_t) + tamanio);

	envio->tamanio = tamanio;
	envio->tamanioUsado = 0;
	envio->contenido = malloc(tamanio);

	return envio;
}

paquete* crearHeader(size_t tamanio, u_int32_t servicio){
	paquete* envio = crearPaquete(sizeof(u_int32_t) + tamanio);

	empaquetar(envio, &servicio, sizeof(u_int32_t));

	return envio;
}

void destruirPaquete(paquete *envio){
	free(envio->contenido);
	free(envio);
}

void empaquetar(paquete* envio, void* datos, size_t tamanioDato){
	int verificador;										// Para verificar que no se están empaquetando más cosas que el paquete puede contener.

	envio->tamanioUsado += tamanioDato;

	verificador = (envio->tamanio) - (envio->tamanioUsado);

	esErrorSimple(verificador, "Se están empaquetando más datos de los disponibles.\n");

	memcpy(envio->contenido, datos, tamanioDato);			// Empaqueta el dato copiándolo en un mensaje.
	envio->contenido += tamanioDato;						// Desplaza el puntero para poder poner el dato que siga a continuación.
}

void empaquetarVariable(paquete* envio, void* datos, size_t tamanioDato){
	empaquetar(envio, &tamanioDato, sizeof(size_t));

	empaquetar(envio, datos, tamanioDato);
}

void empaquetarLista(paquete* envio, t_list* lista, size_t tamanioNodo){
	u_int32_t indice;													// Para recorrer la lista como un array.
	u_int32_t cantidadElementos = list_size(lista);						// Para saber cuántos elementos enviar.

	empaquetar(envio, &cantidadElementos, sizeof(u_int32_t));			// Para que el receptor sepa cuántos nodos va a tener la lista.

	for(indice = 0; indice < cantidadElementos; indice++)
	{
		empaquetar(envio, list_get(lista, indice), tamanioNodo);		// Lo empaqueto para el send().
	}
}

void enviarPaquete(int socket, paquete* envio){
	int verificador = (envio->tamanioUsado) - (envio->tamanio);				// Para verificar que al paquete está completo a la hora de enviar.

	esErrorSimple(verificador, "Faltan empaquetar datos.\n");

	envio->contenido -= envio->tamanio;										// Reubica el puntero del paquete para referenciar a inicio y enviarlo correctamente.

	enviarDato(socket, envio->contenido, envio->tamanio);					// Envía el paquete

	destruirPaquete(envio);													// Libera el paquete.
}

void recibirPaquete(int socket, void* receptor, size_t tamanioRecibo){
	recibirDato(socket, receptor, tamanioRecibo);	// Re parásito XD
}

size_t recibirPaqueteVariable(int socket, void** receptor){
	size_t tamanio;

	recibirDato(socket, &tamanio, sizeof(size_t));

	*receptor = malloc(tamanio);

	recibirDato(socket, *receptor, tamanio);

	return tamanio;
}

size_t recibirLista(int socket, t_list* lista, size_t tamanioNodo){
	u_int32_t indice;				// Para agregar nodos a la lista como un array.
	u_int32_t cantidadElementos;	// Para saber el tope de la lista y cuando parar de añadir objetos a la misma.

	recibirPaquete(socket, &cantidadElementos, sizeof(size_t));	// Recibe el tamaño de la lista.

	for(indice = 0; indice < cantidadElementos; indice++)
	{
		void* nodo = malloc(tamanioNodo);			// Aloco memoria para un nodo.

		recibirPaquete(socket, nodo, tamanioNodo);	// Recibo el nodo.
		list_add(lista, nodo);						// Lo añado a la lista.
	}

	u_int32_t tamanioLista = tamanioEnBytesListaFija(lista, tamanioNodo);

	return tamanioLista;
}

u_int32_t recibirHeader(int socket){
	u_int32_t accion;

	recibirDato(socket, &accion, sizeof(u_int32_t));

	return accion;
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
