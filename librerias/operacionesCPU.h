/*
 * operacionesCPU.h
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */

#ifndef OPERACIONESCPU_H_
#define OPERACIONESCPU_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <commons/collections/list.h>
#include <commons/collections/node.h>

#include <parser/metadata_program.h>

#include "../librerias/controlErrores.h"

#define SIZE_DATA 1024

// Primera aproximación de las funciones que realiza la CPU y sus estructuras de datos.
// Tranquilos, va a haber cambios XD



// Declaraciones


// Estructuras de datos (Cuidado si hay estructuras similares a otros procesos. ¡¡Avisen!!)

// Otra cosa: por ahora dice "int", pero lo mejor es tener cuidado con eso. El tamaño de un "int" depende de la palabra del procesador.
// Lo mejor es siempre usar un "int" con tamaño fijo.
// Lean bien esto: http://www.gnu.org/software/libc/manual/html_node/Integers.html


typedef struct{
	int pag; // Página donde está almacenado un dato en memoria.
	int offset; // Despleazamiento de un dato respecto a la posición inicial de memoria.
	int size; // Tamaño del dato alojado en memoria.
} posicionMemoria; // Modelo de una posición en memoria.


typedef struct{
	t_list* argumentos; // Posiciones de memoria donde se almacenan las copias de los argumentos de la función.
	t_list* variables; // Identificadores y posiciones de memoria donde se almacenan las variables locales de la función.
	int retPos; // Posición del índice de código donde se debe retornar al finalizar la ejecución de la función.
	posicionMemoria retVar; // Posición de memoria donde se debe almacenar el resultado de la función provisto por la sentencia RETURN.
} stack; // Datos de una llamada a función.


typedef struct {
	int pid; // Identificador de un proceso.
	int pc; // Program counter: indica el número de la siguiente instrucción a ejecutarse.
	// int paginasUsadas; // Cantidad de páginas usadas por el programa (Desde 0).
	// uint8_t indiceCodigo[][]; // Identifica línea útil de código a partir de su posición en el programa para convertirla en una posición de memoria para su consulta.
	// ¿diccionario? indiceEtiqueta; // Tiene el valor del PC que necesita una función para ser llamada.
	t_list* indiceStack; // Ordena valores almacenados en la pila de funciones con sus valores.
	int exitCode; // Motivo por el cual finalizó un programa.
} pcb; // El Process Control Block que maneja el Kernel.
// De momento, me limito a los dos datos de los que habla el checkpoint.


char buffer[SIZE_DATA];



// Funciones de comunicación: las clasificamos así porque requieren de interacción entre el CPU con otros procesos.


// Con Kernel

void handshakeKernel(int socketKernel); // Realiza handshake con el Kernel.

pcb recibirPCB(int socketKernel); // Recibe PCB del Kernel para ejecutar un programa.

void concluirOperacion(pcb datosPcb, int socketKernel); // Notificar al Kernel que se terminó la ejecución de una operación para que la pueda seguir otra CPU de ser necesario.

void finEjecucion(int socketKernel); // Indicar finalización de un proceso.

void conectarNuevaCPU(int socketKernel); // Conectar otra CPU al sistema.

void desconectarCPU(int senial, int socketKernel); // ¿Por qué no puedo poner 'ñ'?


// Con Memoria

void handshakeMemoria(int socketMemoria); // Realiza handshake con Memoria.

char* solicitarSentencia(posicionMemoria unaPos, int socketMemoria); // Solicitar sentencia en Memoria.

char* obtenerDatos(posicionMemoria unaPos, int socketMemoria); // Obtiene información de un programa en ejecución.

void actualizarValores(char* nuevosDatos, posicionMemoria unaPos, int socketMemoria); // Actualiza estrtucturas tras una operación.



// Funciones de CPU: las clasificamos así porque son las funciones que componen al CPU para que haga su trabajo.

void interpretarOperacion(); // Recibe una instrucción de un programa y la parsea.

void ejecutarOperacion(); // Ejecuta una instrucción parseada.

void llamarFuncion(/* Stack */); // Llama a una función o procedimiento.

void actualizarPC(int PC, int valor); // Incrementa el Program Counter con la próxima instrucción a ejecutar.

void arrojarExcepcion(/* Excepción */);



// Auxiliares

void limpiarBuffer();


pcb recibirPCB(int socketKernel){
	pcb datosPcb;

	limpiarBuffer();

	int cantidadRecibida = recv(socketKernel, buffer, SIZE_DATA, 0);
	esErrorConSalida(cantidadRecibida, "Error al recibir PCB.");

	// Acá tendría que pasar lo del buffer al PCB.

	return datosPcb;
}

void concluirOperacion(pcb datosPcb, int socketKernel){
	limpiarBuffer();

	// Acá debería setearse el buffer con los datos de la operación que terminó.

	send(socketKernel, buffer, SIZE_DATA, 0);
}

void finEjecucion(int socketKernel){
	limpiarBuffer();

	// Acá debería setearse el buffer con el Exit Code del proceso.

	send(socketKernel, buffer, SIZE_DATA, 0);
}

char* solicitarSentencia(posicionMemoria unaPos, int socketMemoria){
	char* unaSentencia;

	limpiarBuffer();

	// Acá debería setearse el buffer con la posición de la sentencia que se busca en Memoria.

	send(socketMemoria, buffer, SIZE_DATA, 0);

	int cantidadRecibida = recv(socketMemoria, buffer, SIZE_DATA, 0);
	esErrorConSalida(cantidadRecibida, "Error al recibir sentencia.");

	// Acá se devuelve la línea útil que debería ser parseada por la CPU.

	return unaSentencia;
}

char* obtenerDatos(posicionMemoria unaPos, int socketMemoria){
	char* unosDatos;

	limpiarBuffer();

	// Acá debería setearse el buffer con la posición del dato que se busca en Memoria.

	send(socketMemoria, buffer, SIZE_DATA, 0);

	int cantidadRecibida = recv(socketMemoria, buffer, SIZE_DATA, 0);
	esErrorConSalida(cantidadRecibida, "Error al recibir sentencia.");

	// Acá se devuelve dato con el que va a operar la CPU.

	return unosDatos;
}

void actualizarValores(char* nuevosDatos, posicionMemoria unaPos, int socketMemoria){
	limpiarBuffer();

	// Acá se setea el buffer con la posición en Memoria que va a recibir los nuevos datos.

	send(socketMemoria, buffer, SIZE_DATA, 0);
}

void actualizarPC(int PC, int valor){
	PC = valor; // Lo único que debe funcionar correctamente XD
}

void limpiarBuffer(){
	memset (buffer,'\0',SIZE_DATA);
}

#endif /* OPERACIONESCPU_H_ */
