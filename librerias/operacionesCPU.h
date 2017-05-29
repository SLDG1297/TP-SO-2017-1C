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
#include "../librerias/pcb.h"
#include "../librerias/serializador.h"



// Declaraciones

// Funciones de comunicación: las clasificamos así porque requieren de interacción entre el CPU con otros procesos.

// Con Kernel

void 	handshakeKernel(int socketKernel); 								// Realiza handshake con el Kernel.

void 	recibirPCB(int socketKernel, pcb* unPcb); 						// Recibe PCB del Kernel para ejecutar un programa.

void 	concluirOperacion(int socketKernel, pcb* unPcb); 				// Notificar al Kernel que se terminó la ejecución de una operación para que la pueda seguir otra CPU de ser necesario.

void 	finEjecucion(int socketKernel, pcb* unPcb, int32_t codigoFin); 	// Indicar finalización de un proceso.

void 	conectarCPU(int socketKernel); 									// Conectar otra CPU al sistema.

void 	desconectarCPU(int socketKernel, int senial); 					// ¿Por qué no puedo poner 'ñ'?

// Con Memoria

void 	handshakeMemoria(int socketMemoria, u_int32_t* tamanioPaginas); 					// Realiza handshake con Memoria. Obtiene el tamaño de las páginas.

char*	solicitarInstruccion(int socketMemoria, u_int32_t tamanioPaginas, pcb* unPcb); 		// Solicitar instrucción en Memoria.

char* 	obtenerDatos(int socketMemoria, posicionMemoria unaPos); 							// Obtiene información de un programa en ejecución.

int32_t	hallarPagina(u_int32_t posicionInstruccion, u_int32_t tamanioPaginas);				// Actualiza estructuras tras una operación.

// Funciones de CPU: las clasificamos así porque son las funciones que componen al CPU para que haga su trabajo.

void 	interpretarOperacion(); 												// Recibe una instrucción de un programa y la parsea.

void 	ejecutarOperacion(); 													// Ejecuta una instrucción parseada.

void 	llamarFuncion(/* Stack */); 											// Llama a una función o procedimiento.

void 	actualizarPC(int *PC, int32_t valor); 									// Incrementa el Program Counter con la próxima instrucción a ejecutar.

int		hallarPagina(u_int32_t posicionInstruccion, u_int32_t tamanioPaginas);	// Encuentra la página en la que se encuentra una posición de Memoria.

void 	arrojarExcepcion(/* Excepción */); 										// Se explica solo.

// Adaptadores: Funciones que castean void* y los añaden a una lista. Para recibir listas desde el serializador.

void	adaptadorArgumentoStack(int socket, t_list* listaArgumentos, void* nodo); 	// Añadir argumentos a un índice de Stack.

void	adaptadorVariableStack(int socket, t_list* listaVariables, void* nodo);		// Añadir variable a un índice de Stack.

void	adaptadorStack(int socket, t_list* pila, void* nodo);						// Armar un contexto de ejecución de un proceso.



// Definiciones

void recibirPCB(int socketKernel, pcb* unPcb){
	unPcb->pid = *(int32_t*)recibirMensaje(socketKernel);

	unPcb->programCounter = *(int32_t*)recibirMensaje(socketKernel);

	unPcb->paginasUsadas = *(int32_t*)recibirMensaje(socketKernel);

	unPcb->indiceCodigo.primerInstruccion = *(u_int32_t*)recibirMensaje(socketKernel);

	unPcb->indiceCodigo.instruccionesSize = *(u_int32_t*)recibirMensaje(socketKernel);

	unPcb->indiceCodigo.instrucciones = (lineaUtil*)recibirMensaje(socketKernel);

	unPcb->indiceEtiqueta.etiquetasSize = *(t_size*)recibirMensaje(socketKernel);

	unPcb->indiceEtiqueta.etiquetas = (char*)recibirMensaje(socketKernel);

	unPcb->indiceStack = recibirLista(socketKernel, sizeof(indiceDeStack), &adaptadorStack);

	unPcb->exitCode = *(u_int32_t*)recibirMensaje(socketKernel);

}

void handshakeMemoria(int socketMemoria, u_int32_t* tamanioPaginas){
	*tamanioPaginas = *(u_int32_t*)recibirDatos(socketMemoria, sizeof(u_int32_t));
}

char* solicitarInstruccion(int socketMemoria, u_int32_t tamanioPaginas, pcb* unPcb){
	int proximaInstruccion = unPcb->programCounter; // Indica cuál es la proxima instrucción a ejecutar.

	lineaUtil instruccion = unPcb->indiceCodigo.instrucciones[proximaInstruccion];	// Obtengo el la posición de inicio y fin del programa.
	u_int32_t posicionInstruccion = proximaInstruccion * 2 *sizeof(u_int32_t); 		// Obtengo la posición de la instrucción en bytes.

	int32_t pagina = hallarPagina(posicionInstruccion, tamanioPaginas); // Esta es la página en Memoria donde está la instrucción.

	u_int32_t offset = instruccion.offset; 		// Esto es el desplazamiento en bytes del código respecto al inicio del programa escrito.
	u_int32_t longitud = instruccion.longitud; 	// Esto es la longitud en bytes de la instrucción respecto del offset.

	solicitudMemoria* solicitudInstruccion = malloc(sizeof(solicitudMemoria));
	solicitudInstruccion->pid = unPcb->pid;
	solicitudInstruccion->pagina = pagina;
	solicitudInstruccion->offset = offset;
	solicitudInstruccion->size = longitud;
	// Creación de la estructura para enviar solicitud.

	enviarMensaje(socketMemoria, solicitudInstruccion, sizeof(solicitudMemoria));
	free(solicitudInstruccion);
	// Envío de solicitud de instrucción a memoria.

	char* instruccionSolicitada = recibirMensaje(socketMemoria);
	// Obtención de la solicitud.

	return instruccionSolicitada;
}

void actualizarPC(int *PC, int32_t valor){
	*PC = valor;
}

int32_t	hallarPagina(u_int32_t posicionInstruccion, u_int32_t tamanioPaginas){
	int32_t pagina = 0;

	while(posicionInstruccion > (pagina + 1) * tamanioPaginas)
		pagina++;

	return pagina;
}

void adaptadorArgumentoStack(int socket, t_list* listaArgumentos, void* nodo){
	argStack* argumento = (argStack*)nodo;
	list_add(listaArgumentos, argumento);
}

void adaptadorVariableStack(int socket, t_list* listaVariables, void* nodo){
	variableStack* variable = (variableStack*)nodo;
	list_add(listaVariables, variable);
}

void adaptadorStack(int socket, t_list* pila, void* nodo){
	indiceDeStack* unContexto = malloc(sizeof(indiceDeStack));

	void(*adaptadorArgumentos)(int, t_list*, void*) = &adaptadorArgumentoStack;
	void(*adaptadorVariables)(int, t_list*, void*) = &adaptadorVariableStack;

	unContexto->argumentos = recibirLista(socket, sizeof(argStack), adaptadorArgumentos);
	unContexto->variables = recibirLista(socket, sizeof(variableStack), adaptadorVariables);
	unContexto->retPos = *(u_int32_t*)recibirDatos(socket, sizeof(u_int32_t));
	unContexto->retVar = *(posicionMemoria*)recibirDatos(socket, sizeof(posicionMemoria));

	list_add(pila, unContexto);
}

#endif /* OPERACIONESCPU_H_ */
