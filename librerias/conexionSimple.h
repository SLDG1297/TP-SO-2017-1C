/*
 * conexionSimple.h
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#ifndef CONEXIONSIMPLE_H_
#define CONEXIONSIMPLE_H_

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



// Declaraciones

void servir();							// Generar un servidor que admite conexión de otros procesos.

int	conectar(char* ip, int puerto);		// Genera un socket para la conexión de un cliente a un servidor.

// Definiciones

void servir(){

}

int conectar(char* ip, int puerto){
	// Estructuras de datos del servidor al cual se va a conectar el cliente.

	struct sockaddr_in direccionServidor;	// Estructura para representar la dirección del servidor.
	int socketServidor;						// Descriptor de archivo para comunicarse con el servidor.
	int respuestaConexion;					// Verifica que la conexión sea correcta.

	// Se crea el socket para conectarse con el servidor.

	socketServidor = socket(AF_INET, SOCK_STREAM, 0);
	esErrorConSalida(socketServidor, "Error en la creación del Socket.");

	// Asignación de datos a la dirección del servidor.

	direccionServidor.sin_family = AF_INET;					// Dominio de la dirección del servidor.
	direccionServidor.sin_port = htons(puerto);				// Puerto del servidor.
	direccionServidor.sin_addr.s_addr = inet_addr(ip);		// Dirección IP del servidor.
	bzero(&(direccionServidor.sin_zero), 8);				// Para completar la estructura y castear correctamente.

	// Conexión a servidor.

	respuestaConexion = connect(socketServidor, (struct sockaddr *) &direccionServidor, sizeof(struct sockaddr));
	esErrorConSalida(respuestaConexion, "Error en la conexión al servidor.");

	return socketServidor;
}

#endif /* CONEXIONSIMPLE_H_ */
