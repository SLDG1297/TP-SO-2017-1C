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
// ./conexionSimpleCliente



// Estructuras de datos

typedef void** paquete;

struct estructura{
	u_int32_t numero;
	char letra;
};



// Variables globales

char* ipServidor = "127.0.0.1";

int puertoServidor = 5100;

int socketServidor;

int testCliente = 0;



// Para incrementar tests

void proximoTest(char* enunciado);



// Tests

void pruebaConexion();

void serializar();

void serializarPorLasDudas();

void serializarTamanioVariableFeo();

void serializarTamanioVariable();


// Implementación de tests

void pruebaConexion(){
	u_int32_t datoAEnviar = 4;

	send(socketServidor, &datoAEnviar, sizeof(u_int32_t), 0);



	// Aserciones

	printf("Se pudo enviar el número 4 correctamente.\n\n");
}

void serializar(){
	struct estructura emisor;
	emisor.numero = 5;
	emisor.letra = 'b';

	size_t tamanioEnvio = sizeof(struct estructura);
	printf("Tamanio de envío = %d\n", tamanioEnvio);
	// Serialización (Los comentarios debajo de las funciones indican masomenos qué está pasando ahí atrás)

	// Creación de paquete
	paquete envio = crearPaquete(tamanioEnvio);

	// void* envio = malloc(tamanioEnvio);

	// Empaquetado

	empaquetar(envio, &emisor.numero, sizeof(size_t));

	// memcpy(envio, &emisor.numero, sizeof(size_t));
	// envio += sizeof(size_t);

	empaquetar(envio, &emisor.letra, sizeof(char));

	// memcpy(envio, &emisor.letra, sizeof(char));
	// envio += sizeof(char);

	// >>Envío

	enviarPaquete(socketServidor, envio, tamanioEnvio);

	// *envio -= tamanioEnvio;
	// send(socketServidor, *envio, tamanioEnvio, 0);

	// Destrucción del envío
	// free(*envio);



	// Aserciones

	printf("Se pudo serializar la estructura (int = 5, char ='b') correctamente.\n\n");
}

void serializarPorLasDudas(){
	float numero = 30.0;

	paquete envio = crearPaquete(sizeof(float));

	empaquetar(envio, &numero, sizeof(float));

	enviarPaquete(socketServidor, envio, sizeof(float));



	// Aserciones

	printf("Se envió el flotante 30.0 correctamente.\n\n");
}

void serializarTamanioVariableFeo(){
	u_int32_t* vector = calloc(2, sizeof(u_int32_t));
	size_t tamanio = 2 * sizeof(u_int32_t);

	vector[0] = 300;
	vector[1] = 500;

	paquete envio = crearPaquete(sizeof(size_t) + tamanio);

	empaquetar(envio, &tamanio, sizeof(size_t));

	empaquetar(envio, vector, tamanio);

	enviarPaquete(socketServidor, envio, sizeof(size_t) + tamanio);



	// Aserciones

	printf("Se pudo enviar el vector (300, 500) correctamente.\n");
}

void serializarTamanioVariable(){
	char* cadena = "Ripeando Ando";
	printf("Cadena %s creada.\n", cadena);

	size_t tamanioCadena = tamanioEnBytesString(cadena);
	printf("Tamanio Cadena = %d\n", tamanioCadena);

	size_t tamanioReal = tamanioEnBytesVariables(1) + tamanioCadena;
	printf("Tamanio Envío = %d\n", tamanioReal);
	printf("Ok");
	paquete envio = crearPaquete(tamanioReal);
	printf("Paquete creado\n");

	empaquetarVariable(envio, cadena, tamanioCadena);
	printf("Cadena empaquetada\n");

	enviarPaquete(socketServidor, envio, tamanioReal);
	printf("Se pudo enviar el paquete correctamente.\n");

}

void proximoTest(char* enunciado){
	testCliente++;
	printf("\n\nTest %d\n\n", testCliente);
	printf("%s\n\n", enunciado);
}



// Resultados de las pruebas

int main(){
	socketServidor = conectar(ipServidor, puertoServidor);

	proximoTest("Que se pueda mandar el número 4 por socket.");
	pruebaConexion();

	proximoTest("Que se pueda serializar la estructura compuesta (int = 5, char ='b').");
	//serializar();

	proximoTest("Para que vean que no mandé fruta con el tema de enviar paquetes, envío el flotante 30.0");
	//serializarPorLasDudas();

	proximoTest("Que se serialice el vector (300, 500) codeando feo.");
	serializarTamanioVariableFeo();

	proximoTest("Que se pueda serializar el string Ripeando Ando.");
	serializarTamanioVariable();

}
