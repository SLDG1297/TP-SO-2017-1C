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

struct estructura{
	u_int32_t numero;
	char letra;
};

struct parOrdenado{
	float x;
	float y;
} __attribute__((packed));

struct nodoFijo{
	char letra;
	u_int32_t entero;
	float flotante;
} __attribute__((packed));



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

void serializarTamanioVariableQueNoEsString();

void serializarListaFija();



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

	size_t tamanioEnvio = sizeof(u_int32_t) + sizeof(char);

	paquete* envio = crearPaquete(tamanioEnvio);

	empaquetar(envio, &emisor.numero, sizeof(size_t));

	empaquetar(envio, &emisor.letra, sizeof(char));

	enviarPaquete(socketServidor, envio);



	// Aserciones

	printf("Se pudo serializar la estructura (int = 5, char ='b') correctamente.\n\n");
}

void serializarPorLasDudas(){
	float numero = 30.0;
	u_int32_t accion = 3;

	paquete* envio = crearHeader(sizeof(float), accion);

	empaquetar(envio, &numero, sizeof(float));

	enviarPaquete(socketServidor, envio);



	// Aserciones

	printf("Se envió el flotante 30.0 correctamente para realizar la operación 3.\n\n");
}

void serializarTamanioVariableFeo(){
	u_int32_t* vector = calloc(2, sizeof(u_int32_t));
	size_t tamanio = 2 * sizeof(u_int32_t);

	vector[0] = 300;
	vector[1] = 500;

	paquete* envio = crearPaquete(sizeof(size_t) + tamanio);

	empaquetar(envio, &tamanio, sizeof(size_t));

	empaquetar(envio, vector, tamanio);

	enviarPaquete(socketServidor, envio);



	// Aserciones

	printf("Se pudo enviar el vector (300, 500) correctamente.\n\n");
}

void serializarTamanioVariable(){
	char* cadena = "Ripinpin";
	size_t tamanioCadena = tamanioEnBytesString(cadena);

	size_t tamanioReal = tamanioEnBytesVariables(1) + tamanioCadena;

	paquete* envio = crearPaquete(tamanioReal);

	empaquetarVariable(envio, cadena, tamanioCadena);

	enviarPaquete(socketServidor, envio);

	printf("Se pudo enviar el paquete correctamente.\n\n");

}

void serializarTamanioVariableQueNoEsString(){
	struct parOrdenado* vector = calloc(2, 2 * sizeof(float));
	size_t tamanioVector = 2 * 2 * sizeof(float);

	vector[0].x = 500.30;	// Que forma más extraña de armar la matriz...
	vector[0].y = 200.25;
	vector[1].x = 666.66;
	vector[1].y = 999.99;

	size_t tamanioPaquete = sizeof(size_t) + tamanioVector;

	paquete* envio = crearPaquete(tamanioPaquete);

	empaquetarVariable(envio, vector, tamanioVector);

	enviarPaquete(socketServidor, envio);



	// Aserciones

	printf("Se pudo enviar el vector correctamente.\n\n");

}

void serializarListaFija(){
	// Guarda. Si van a enviar el nodo, tiene que estar con __attribute__((packed)). Sino, va a haber padding.

	struct nodoFijo elemento1;
	elemento1.letra = 'a';
	elemento1.entero = 1;
	elemento1.flotante = 30.0;

	struct nodoFijo elemento2;
	elemento2.letra = 'b';
	elemento2.entero = 500;
	elemento2.flotante = 0.5;

	struct nodoFijo elemento3;
	elemento3.letra = 'z';
	elemento3.entero = 666;
	elemento3.flotante = 676.1;

	size_t tamanioNodo = sizeof(struct nodoFijo);

	t_list* lista = list_create();

	list_add(lista, &elemento1);
	list_add(lista, &elemento2);
	list_add(lista, &elemento3);

	size_t tamanioLista = tamanioEnBytesListaFija(lista, tamanioNodo);

	size_t tamanioEnvio = sizeof(u_int32_t) + tamanioLista;

	paquete* envio = crearPaquete(tamanioEnvio);

	empaquetarLista(envio, lista, tamanioNodo);

	enviarPaquete(socketServidor, envio);

	list_destroy(lista);



	// Aserciones

	printf("Se pudo empaquetar la lista correctamente.\n\n");
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
	serializar();

	proximoTest("Que se envíe el flotante 30.0 solicitando realizar la operación 3.");
	serializarPorLasDudas();

	proximoTest("Que se serialice el vector (300, 500) codeando feo.");
	serializarTamanioVariableFeo();

	proximoTest("Que se pueda deserializar el string Ripinpin.");
	serializarTamanioVariable();

	proximoTest("No todo es char* en la vida. Probemos con el vector ((500.30, 200.25), (666.66, 999.99))");
	serializarTamanioVariableQueNoEsString();

	proximoTest("Que se pueda serializar la lista [(a, 1, 30.0) , (b, 500, 0.5), (z, 666, 676.1)].");
	serializarListaFija();
}
