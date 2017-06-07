/*
 * conexionSimpleServidor.c
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#include "../librerias/conexionSocket.h"
#include "../librerias/serializador.h"

// Línea para compilar:
// gcc conexionSimpleServidor.c -o conexionSimpleServidor -lcommons
// ./conexionSimpleServidor



// Estructuras de datos

struct estructura{
	u_int32_t numero;
	char letra;
} __attribute__((packed));



// Variables globales

int puertoServidor = 5100;

int socketCliente;

int testServidor = 0;



// Para incrementar tests

void proximoTest();



// Tests

void pruebaServidor();

void deserializar();



// Implementación de tests

void pruebaServidor(){
	proximoTest();
	printf("Prueba de servidor\n");

	u_int32_t datoRecibido;

	recv(socketCliente, &datoRecibido, sizeof(u_int32_t), 0);

	if(datoRecibido == 4)
		printf("\nSe recibió el dato correctamente.\n");
	else
		printf("\nQue en paz descanses...\n");
}

void deserializar(){
	proximoTest();
	printf("Prueba de deserializador\n");
	// Contexto del receptor:

	struct estructura receptor;

	// Deserialización

	// <<Recibo

	recibirPaquete(socketCliente, &receptor.numero, sizeof(receptor.numero));

	recibirPaquete(socketCliente, &receptor.letra, sizeof(receptor.letra));


	if(receptor.numero == 5)
		printf("Se obtuvo el número correcto 5.\n");
	else
		printf("Se esperaba 5 y obtuviste %d\n", receptor.numero);

	if(receptor.letra == 'b')
		printf("Se obtuvo la letra correcta 'b'.\n\n");
	else
		printf("Se esperaba b y obtuviste %c.\n\n", receptor.letra);
}

void proximoTest(){
	testServidor++;
	printf("\n\nTest %d\n\n", testServidor);
}

// Resultado de las pruebas

int main(){
	socketCliente = servir(puertoServidor);

	pruebaServidor();	// Que se pueda recibir el número 4 por socket.

	deserializar();		// Que se pueda deserializar la estructura compuesta (int = 5, char ='b').

}
