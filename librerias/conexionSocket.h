/*
 * conexionSimple.h
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#ifndef CONEXIONSOCKET_H_
#define CONEXIONSOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/controlErrores.h"



// Estructuras de datos

typedef struct sockaddr_in dirSock;
// Me tiene re podrido poner struct sockaddr_in. Es una estructura con la dirección de IP asociada a un socket.


// Declaraciones

// Funciones necesarias para conectarse

int 		crearSocket();															// Crea un socket para la conexión entre dos programas (Llama a socket()).

void		configurarSocket(int socketAEditar);									// Modifica las opciones de un Socket (El famoso setsockopt()).

dirSock 	crearDireccion(char* ip, int puerto);									// Genera una estructura para asignar datos para la conexión de un socket.

void		asociarDireccion(int socketABindear, dirSock direccionABindear);		// Liga un socket a una dirección (El bind()).

void		esperarConexion(int socketEnEscucha);									// El socket se habilita para escuchar una conexión (El listen()).

int			aceptarConexion(int socketServidor, dirSock direccionCliente);			// Crea un socket para recibir y enviar datos con un cliente (El accept()).

void 		conectarSocket(int socketAConectar, dirSock direccionAConectar);		// Asocia un socket para recibir y enviar datos. (El connect()).

void		cerrarSocket(int socketACerrar);										// Cierra la conexión del socket para no recibir ni enviar más datos. (El close()).

// Funciones de conexión integradas

int		servir(int puertoCliente);							// Generar un socket que admite conexión de otros procesos.

int		conectar(char* ipServidor, int puertoServidor);		// Genera un socket para la conexión de un cliente a un servidor.

// Definiciones

int crearSocket(){
	int socketNuevo = socket(AF_INET,SOCK_STREAM,0);
	esErrorConSalida(socketNuevo, "Fallo en la creación del socket.");

	printf("Socket creado.\n");

	return socketNuevo;
}

void configurarSocket(int socketAEditar){
	int verificador;
	int yes = 1;

	verificador = setsockopt(socketAEditar, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	esErrorConSalida(verificador,"Error en la configuración del socket.");

	printf("Opciones de socket configruadas correctamente.\n");
}

dirSock crearDireccion(char* ip, int puerto){
	dirSock direccion;

	direccion.sin_family = AF_INET;					// Dominio de la dirección.
	direccion.sin_port = htons(puerto);				// Asignación de Puerto (Se convierte de orden host a network en short).
	direccion.sin_addr.s_addr = inet_addr(ip);		// Asignación de Dirección IP.
	bzero(&(direccion.sin_zero), 8);				// Para completar la estructura y castear correctamente.

	return direccion;
}

void asociarDireccion(int socketABindear, dirSock direccionABindear){
	int verificador;

	verificador = bind(socketABindear, (struct sockaddr *) &direccionABindear, sizeof(struct sockaddr));
	esErrorConSalida(verificador,"Error en la asociación de dirección.");

	printf("Asociación correcta de dirección.\n");
}

void esperarConexion(int socketEnEscucha){
	int verificador;

	verificador = listen(socketEnEscucha, 1);
	esErrorConSalida(verificador,"Error en la escucha de conexiones.");

	printf("Esperando una conexión.\n");
}

int aceptarConexion(int socketServidor, dirSock direccionCliente){
	int socketCliente;
	socklen_t tamanio = sizeof(direccionCliente);

	socketCliente = accept(socketServidor, (struct sockaddr *) &direccionCliente, &tamanio);
	esErrorConSalida(socketCliente,"Error en la acpetación de la conexión.");

	printf("Conexión al servidor aceptada.\n");

	return socketCliente;
}

void conectarSocket(int socketAConectar, dirSock direccionAConectar){
	int verificador;

	verificador = connect(socketAConectar, (struct sockaddr *) &direccionAConectar, sizeof(struct sockaddr));
	esErrorConSalida(verificador, "Error en la conexión al servidor.");

	printf("Conexión a servidor correcta.\n");
}

void cerrarSocket(int socketACerrar){
	int verificador;

	close(socketACerrar);
	esErrorConSalida(verificador, "Error en la cerrar el socket.");

	printf("El socket de escucha cumplió su función y se cerró.");
}

int servir(int puertoCliente){
	int socketServidor;
	dirSock direccionServidor;

	int socketCliente;
	dirSock direccionCliente;

	socketServidor = crearSocket();									// Llamar a socket().
	configurarSocket(socketServidor);								// Llamar a setsockopt().
	direccionServidor = crearDireccion(INADDR_ANY, puertoCliente);	// Asignar estructura de dirección.

	asociarDireccion(socketServidor, direccionServidor);			// Llamar a bind().

	esperarConexion(socketServidor);								// Llamar a listen().

	aceptarConexion(socketServidor, direccionCliente);				// Llamar a accept().

	cerrarSocket(socketServidor);									// Llamar a close().

	return socketCliente;
}

int conectar(char* ipServidor, int puertoServidor){
	int socketServidor;												// Descriptor de archivo para comunicarse con el servidor.
	dirSock direccionServidor;										// Estructura para representar la dirección del servidor.

	socketServidor = crearSocket();									// Se crea el socket para conectarse con el servidor.

	direccionServidor = crearDireccion(ipServidor, puertoServidor);	// Asignación de datos a la dirección del servidor.

	conectarSocket(socketServidor, direccionServidor);				// Conexión a servidor. Llama a connect().

	return socketServidor;
}

#endif /* CONEXIONSOCKET_H_ */
