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
	u_int32_t					pagina;
	u_int32_t 	                offset;
	u_int32_t 					size;
} posicionMemoria;

//Estructura para manejar una posición de memoria.

typedef posicionMemoria argStack;

//Estructura de la lista de argumentos de una función que hay en el stack

typedef struct{
	t_nombre_variable 	nombre;
	posicionMemoria 	posicionMemoria;
} variableStack ;

//Estructura de la lista de variables que hay en el stack

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
	t_list*			argumentos; // Posiciones de memoria donde se almacenan las copias de los argumentos de la función.
	t_list*			variables; 	// Identificadores y posiciones de memoria donde se almacenan las variables locales de la función.
	t_puntero		retPos; 	// Posición del índice de código donde se debe retornar al finalizar la ejecución de la función.
	posicionMemoria	retVar; 	// Posición de memoria donde se debe almacenar el resultado de la función provisto por la sentencia RETURN.
} indiceStack;

// El índice de Stack del PCB para poder hacer llamadas a procedimientos con sus argumentos.

typedef struct{
	int 				pid; 				// Identificador de un proceso.
	int 				programCounter; 				// Program counter: indica el número de la siguiente instrucción a ejecutarse.
	int 				paginasUsadas; 		// Cantidad de páginas usadas por el programa (Desde 0).
	t_list*		        indiceCodigo;		// Identifica líneas útiles de un programa
	t_list*		        indiceEtiqueta;		// Identifica llamadas a funciones.
	t_list*		        indiceStack; 		// Ordena valores almacenados en la pila de funciones con sus valores.
	int 				exitCode; 			// Motivo por el cual finalizó un programa.
} pcb;

// El Process Control Block que maneja el Kernel y tiene toda la información del proceso que ejecuta la CPU.

// Declaraciones

// Funciones de comunicación: las clasificamos así porque requieren de interacción entre el CPU con otros procesos.

// Con Kernel

void 	handshakeKernel(int socketKernel); 							// Realiza handshake con el Kernel.

void 	recibirPCB(int socketKernel, pcb* unPcb); 					// Recibe PCB del Kernel para ejecutar un programa.

void 	concluirOperacion(int socketKernel, pcb* unPcb); 			// Notificar al Kernel que se terminó la ejecución de una operación para que la pueda seguir otra CPU de ser necesario.

void 	finEjecucion(int socketKernel, pcb* unPcb, int codigoFin); 	// Indicar finalización de un proceso.

void 	conectarCPU(int socketKernel); 								// Conectar otra CPU al sistema.

void 	desconectarCPU(int senial, int socketKernel); 				// ¿Por qué no puedo poner 'ñ'?

// Con Memoria

void 	handshakeMemoria(int socketMemoria, int* tamanioPaginas); 								// Realiza handshake con Memoria. Obtiene el tamaño de las páginas.

char*	solicitarInstruccion(int socketMemoria, int tamanioPaginas, pcb* unPcb); 				// Solicitar instrucción en Memoria.

char* 	obtenerDatos(int socketMemoria, posicionMemoria unaPos); 								// Obtiene información de un programa en ejecución.

void 	actualizarValores(char* nuevosDatos, posicionMemoria unaPos, int socketMemoria); 		// Actualiza estructuras tras una operación.

// Funciones de CPU: las clasificamos así porque son las funciones que componen al CPU para que haga su trabajo.

void 	interpretarOperacion(); 														// Recibe una instrucción de un programa y la parsea.

void 	ejecutarOperacion(); 															// Ejecuta una instrucción parseada.

void 	llamarFuncion(/* Stack */); 													// Llama a una función o procedimiento.

void 	actualizarPC(int *PC, int valor); 												// Incrementa el Program Counter con la próxima instrucción a ejecutar.

int		hallarPagina(t_puntero_instruccion posicionInstruccion, int tamanioPaginas);	// Encuentra la página en la que se encuentra una posición de Memoria.

void 	arrojarExcepcion(/* Excepción */); 												// Se explica solo.



// Definiciones

void recibirPCB(int socketKernel, pcb* unPcb){
	unPcb->pid = *(int*)recibirMensaje(socketKernel);

	unPcb->pc = *(int*)recibirMensaje(socketKernel);

	unPcb->paginasUsadas = *(int*)recibirMensaje(socketKernel);

	unPcb->indiceCodigo = *(indiceCodigo*)recibirMensaje(socketKernel);

	unPcb->indiceEtiqueta = *(indiceEtiqueta*)recibirMensaje(socketKernel);

	// Falta el indice de stack. Después lo pienso.

	unPcb->exitCode = *(int*)recibirMensaje(socketKernel);

}

void concluirOperacion(int socketKernel, pcb* unPcb){

}

void finEjecucion(int socketKernel, pcb* unPcb, int codigoFin){

}

void handshakeMemoria(int socketMemoria, int* tamanioPaginas){
	*tamanioPaginas = *(int*)recibirDatos(socketMemoria, sizeof(int));
}

char* solicitarInstruccion(int socketMemoria, int tamanioPaginas, pcb* unPcb){
	int proximaInstruccion = unPcb->pc; // Indica cuál es la proxima instrucción a ejecutar.

	t_intructions instruccion = unPcb->indiceCodigo.instrucciones[proximaInstruccion];		// Obtengo el la posición de inicio y fin del programa.
	t_puntero_instruccion posicionInstruccion = proximaInstruccion * sizeof(t_intructions); // Obtengo la posición de la instrucción en bytes.

	int pagina = hallarPagina(posicionInstruccion, tamanioPaginas); // Esta es la página en Memoria donde está la instrucción.
	t_puntero_instruccion offset = instruccion.start; 				// Esto es el desplazamiento en bytes del código respecto al inicio del programa escrito.
	t_size longitud = instruccion.offset; 							// Esto es la longitud en bytes de la instrucción respecto del offset.

	int* PID = unPcb->pid;

	posicionMemoria* solicitudInstruccion = malloc(sizeof(posicionMemoria));
	solicitudInstruccion->pagina = &pagina;
	solicitudInstruccion->offset = &offset;
	solicitudInstruccion->size = &longitud;
	// Creación de la estructura para enviar solicitud.

	enviarMensaje(socketMemoria, PID, sizeof(int));
	enviarMensaje(socketMemoria, solicitudInstruccion, sizeof(posicionMemoria));
	// Envío de solicitud de instrucción a memoria. Lo hice así por comodidad XD

	char* instruccionSolicitada = recibirMensaje(socketMemoria);
	// Obtención de la solicitud.

	return instruccionSolicitada;
}

void actualizarPC(int *PC, int valor){
	*PC = valor;
}

int	hallarPagina(t_puntero_instruccion posicionInstruccion, int tamanioPaginas){
	int pagina = 0;

	while(posicionInstruccion > (pagina + 1) * tamanioPaginas)
		pagina++;

	return pagina;
}

#endif /* OPERACIONESCPU_H_ */
