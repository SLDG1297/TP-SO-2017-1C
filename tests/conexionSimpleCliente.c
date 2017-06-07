/*
 * conexionSimpleCliente.c
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#include "../librerias/conexionSocket.h"

// Línea para compilar:
// gcc conexionSimpleCliente.c -o conexionSimpleCliente -lcommons
// ./conexionSimpleCliente.c



// Estructuras de datos

struct estructura{
	u_int32_t numero;
	char letra;
} __attribute__((packed));



// Variables globales

char* ipServidor = "127.0.0.1";

int puertoServidor = 5100;

int socketServidor;



// Tests

void pruebaConexion();

void serializar();



// Implementación de tests

void pruebaConexion(){
	printf("\nPrueba de conexion\n");

	u_int32_t datoAEnviar = 4;

	send(socketServidor, &datoAEnviar, sizeof(u_int32_t), 0);

	printf("Se pudo enviar el mensaje correctamente.\n");
}

void serializar(){
	printf("\nPrueba de serializador\n");

	// Contexto del emisor:

	struct estructura emisor;
	emisor.numero = 5;
	emisor.letra = 'b';

	u_int32_t tamanioEnvio = sizeof(struct estructura);

	// Serialización

	// Creación de paquete
	void* envio = malloc(tamanioEnvio);
	u_int32_t desplazamientoEnvio = 0;

	// Empaquetado
	memcpy(envio + desplazamientoEnvio, &emisor.numero, sizeof(u_int32_t));
	desplazamientoEnvio += sizeof(u_int32_t);

	memcpy(envio + desplazamientoEnvio, &emisor.letra, sizeof(char));
	desplazamientoEnvio += sizeof(char);

	// >>Envío
	send(socketServidor, envio, tamanioEnvio, 0);

	// Destrucción del envío
	free(envio);

	printf("\nSe pudo serializar la estructura (int = 5, char ='b') correctamente.\n");
}



// Resultados de las pruebas

int main(){
	socketServidor = conectar(ipServidor, puertoServidor);

	pruebaConexion();	// Que se pueda mandar el número 4 por socket.

	serializar();		// Que se pueda serializar la estructura compuesta (int = 5, char ='b').

}
