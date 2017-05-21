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
#include <commons/collections/dictionary.h>
#include <commons/collections/node.h>

#include <parser/metadata_program.h>

#include "../librerias/controlErrores.h"
#include "../librerias/serializador.h"

#define SIZE_DATA 1024

// Estructuras de datos

typedef struct{
	t_puntero_instruccion	primerInstruccion;	// El numero de la primera instrucción (Begin).
	t_size					instruccionesSize;	// Cantidad de instrucciones del programa.
	t_intructions*			instrucciones; 		// Instrucciones del programa.
} indiceCodigo;

// El índice de Código del PCB con las líneas útiles de código.

typedef struct{
	t_size			etiquetasSize;		// Tamaño del mapa serializado de etiquetas.
	char*			etiquetas;			// La serializacion de las etiquetas.
	int				cantidadFunciones;
	int				cantidadEtiquetas;
} indiceEtiqueta;

// El índice de Etiqueta del PCB para poder identificar las funciones de un programa.

typedef struct{
	t_list 			argumentos; // Posiciones de memoria donde se almacenan las copias de los argumentos de la función.
	t_list 			variables; 	// Identificadores y posiciones de memoria donde se almacenan las variables locales de la función.
	t_puntero 		retPos; 	// Posición del índice de código donde se debe retornar al finalizar la ejecución de la función.
	t_intructions	retVar; 	// Posición de memoria donde se debe almacenar el resultado de la función provisto por la sentencia RETURN.
} indiceStack;

// El índice de Stack del PCB para poder hacer llamadas a procedimientos con sus argumentos.

typedef struct {
	int 				pid; 					// Identificador de un proceso.
	int 				pc; 					// Program counter: indica el número de la siguiente instrucción a ejecutarse.
	int 				paginasUsadas; 			// Cantidad de páginas usadas por el programa (Desde 0).
	indiceCodigo		indiceCodigo;			// Identifica líneas útiles de un programa
	indiceEtiqueta		indiceEtiqueta;			// Identifica llamadas a funciones.
	indiceStack			indiceStack; 			// Ordena valores almacenados en la pila de funciones con sus valores.
	int 				exitCode; 				// Motivo por el cual finalizó un programa.
} pcb;

// El Process Control Block que maneja el Kernel y tiene toda la información del proceso que ejecuta la CPU.

// Declaraciones

// Funciones de comunicación: las clasificamos así porque requieren de interacción entre el CPU con otros procesos.

// Con Kernel

void 	handshakeKernel(int socketKernel); 					// Realiza handshake con el Kernel.

pcb 	recibirPCB(int socketKernel); 						// Recibe PCB del Kernel para ejecutar un programa.

void 	concluirOperacion(int socketKernel, pcb datosPcb); 	// Notificar al Kernel que se terminó la ejecución de una operación para que la pueda seguir otra CPU de ser necesario.

void 	finEjecucion(int socketKernel); 					// Indicar finalización de un proceso.

void 	conectarNuevaCPU(int socketKernel); 				// Conectar otra CPU al sistema.

void 	desconectarCPU(int senial, int socketKernel); 		// ¿Por qué no puedo poner 'ñ'?

// Con Memoria

void 	handshakeMemoria(int socketMemoria); 													// Realiza handshake con Memoria.

char* 	solicitarSentencia(/*posicionMemoria unaPos,*/ int socketMemoria); 						// Solicitar sentencia en Memoria.

char* 	obtenerDatos(/*posicionMemoria unaPos,*/, int socketMemoria); 							// Obtiene información de un programa en ejecución.

void 	actualizarValores(char* nuevosDatos, /*posicionMemoria unaPos,*/, int socketMemoria); 	// Actualiza estructuras tras una operación.

int		paginaEnMemoria(int instruccion, int cantidadPaginas, int tamanioPaginas);				// Devuelve la página en Memoria donde se encuentra la instrucción.

// Funciones de CPU: las clasificamos así porque son las funciones que componen al CPU para que haga su trabajo.

void 	interpretarOperacion(); 			// Recibe una instrucción de un programa y la parsea.

void 	ejecutarOperacion(); 				// Ejecuta una instrucción parseada.

void 	llamarFuncion(/* Stack */); 		// Llama a una función o procedimiento.

void 	actualizarPC(int *PC, int valor); 	// Incrementa el Program Counter con la próxima instrucción a ejecutar.

void 	arrojarExcepcion(/* Excepción */); 	// Se explica solo.

// Definiciones

pcb recibirPCB(int socketKernel){
	pcb unPcb;

	void* pcbRecibido = recibirDatos(socketKernel);

	unPcb.pid = *((int*)pcbRecibido);
	unPcb.pc = *((int*)pcbRecibido + 1);
	unPcb.paginasUsadas = *((int*)pcbRecibido + 2);
	// Naaa... Qué es esto??


	return unPcb;
}

void actualizarPC(int *PC, int valor){
	*PC = valor;
}

#endif /* OPERACIONESCPU_H_ */
