/*
 * conexionSimpleServidor.c
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#include "../librerias/conexionSocket.h"

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



// Tests

void pruebaServidor();

void deserializar();



// Implementación de tests

void pruebaServidor(){
	printf("\nPrueba de servidor\n");

	u_int32_t datoRecibido;

	recv(socketCliente, &datoRecibido, sizeof(u_int32_t), 0);

	if(datoRecibido == 4)
		printf("Se recibió el dato correctamente.\n");
	else
		printf("Que en paz descanses...\n");
}

void deserializar(){
	printf("\nPrueba de deserializador\n");
	// Contexto del receptor:

	struct estructura receptor;

	u_int32_t tamanioRecibo = sizeof(struct estructura);

	// Deserialización

	// <<Recibo

	void* recibo = malloc(tamanioRecibo);
	u_int32_t desplazamientoRecibo = 0;

	recv(socketCliente, recibo, tamanioRecibo, 0);

	// Desempaquetado
	memcpy(&receptor.numero, recibo + desplazamientoRecibo, sizeof(u_int32_t));
	desplazamientoRecibo += sizeof(u_int32_t);

	memcpy(&receptor.letra, recibo + desplazamientoRecibo, sizeof(char));
	desplazamientoRecibo += sizeof(char);

	free(recibo);

	if(receptor.numero == 5)
		printf("\nSe obtuvo el número correcto. (5)\n");
	else
		printf("\nSe esperaba 5 y obtuviste %d", receptor.numero);

	if(receptor.letra == 'b')
		printf("\nSe obtuvo la letra correcta. (b)\n");
	else
		printf("\nSe esperaba b y obtuviste %c\n", receptor.letra);
}



// Resultado de las pruebas

int main(){
	socketCliente = servir(puertoServidor);

	pruebaServidor();	// Que se pueda recibir el número 4 por socket.

	deserializar();		// Que se pueda deserializar la estructura compuesta (int = 5, char ='b').

}
