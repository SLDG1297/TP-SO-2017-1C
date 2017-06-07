/*
 * conexionSimpleCliente.c
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#include "../librerias/conexionSocket.h"
#include "../librerias/serializador.h"

// Línea para compilar:
// gcc conexionSimpleCliente.c -o conexionSimpleCliente -lcommons
// ./conexionSimpleCliente.c



// Estructuras de datos

typedef void** paquete;

struct estructura{
	u_int32_t numero;
	char letra;
} __attribute__((packed));



// Variables globales

char* ipServidor = "127.0.0.1";

int puertoServidor = 5100;

int socketServidor;

int testCliente = 0;



// Para incrementar tests

void proximoTest();




// Tests

void pruebaConexion();

void serializar();



// Implementación de tests

void pruebaConexion(){
	proximoTest();
	printf("Prueba de conexion\n\n");

	u_int32_t datoAEnviar = 4;

	send(socketServidor, &datoAEnviar, sizeof(u_int32_t), 0);



	// Aserciones

	printf("Se pudo enviar el mensaje correctamente.\n\n");
}

void serializar(){
	proximoTest();
	printf("Prueba de serializador\n\n");

	// Contexto del emisor:

	struct estructura emisor;
	emisor.numero = 5;
	emisor.letra = 'b';

	u_int32_t tamanioEnvio = sizeof(struct estructura);

	// Serialización (Los comentarios debajo de las funciones indican qué está pasando ahí atrás)

	// Creación de paquete

	paquete envio = crearPaquete(tamanioEnvio);

	// void* envio = malloc(tamanioEnvio);

	// Empaquetado

	empaquetar(envio, &emisor.numero, sizeof(u_int32_t));

	// memcpy(envio, &emisor.numero, sizeof(u_int32_t));
	// envio += sizeof(u_int32_t);

	empaquetar(envio, &emisor.letra, sizeof(char));

	// memcpy(envio, &emisor.letra, sizeof(char));
	// envio += sizeof(char);

	// >>Envío

	enviarPaquete(socketServidor, envio, tamanioEnvio);

	// *envio -= tamanioEnvio;
	// send(socketServidor, *envio, tamanioEnvio, 0);

	// Destrucción del envío
	// free(*envio);*/



	// Aserciones

	printf("Se pudo serializar la estructura (int = 5, char ='b') correctamente.\n\n");
}

void proximoTest(){
	testCliente++;
	printf("\n\nTest %d\n\n", testCliente);
}


// Resultados de las pruebas

int main(){
	socketServidor = conectar(ipServidor, puertoServidor);

	pruebaConexion();	// Que se pueda mandar el número 4 por socket.

	serializar();		// Que se pueda serializar la estructura compuesta (int = 5, char ='b').

}
