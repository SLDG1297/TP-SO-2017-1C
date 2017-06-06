/*
 * operacionesCPU.h
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */

#ifndef FUNCIONESCPU_H_
#define FUNCIONESCPU_H_

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

// Protocolo de comunicación de CPU
#define FIN_OPERACION 	1297
#define	FIN_PROGRAMA	1298
#define	EXCEPCION		1299
#define DESCONECTAR 	1300

// Declaraciones

// Funciones de comunicación: las clasificamos así porque requieren de interacción entre el CPU con otros procesos.

// Con Kernel

void 		handshakeKernel(int socketKernel, u_int32_t codigoCPU); 									// Realiza handshake con el Kernel.

void 		concluirOperacion(int socketKernel, pcb* unPcb); 											// Notificar al Kernel que se terminó la ejecución de una operación para que la pueda seguir otra CPU de ser necesario.

void 		finEjecucion(int socketKernel, pcb* unPcb, int32_t codigoFin); 								// Indicar finalización de un proceso.

void 		conectarCPU(int socketKernel); 																// Conectar otra CPU al sistema.

void 		desconectarCPU(int socketKernel, int senial); 												// ¿Por qué no puedo poner 'ñ'?

// Con Memoria

void 		handshakeMemoria(int socketMemoria, u_int32_t codigoCPU, u_int32_t* tamanioPaginas); 		// Realiza handshake con Memoria. Obtiene el tamaño de las páginas.

char* 		fetchInstruction(int socketMemoria, solicitudMemoria instruccion);							// Solicitar instrucción en Memoria.

char* 		obtenerDatos(int socketMemoria, posicionMemoria unaPos); 									// Obtiene información de un programa en ejecución.

// Funciones de CPU: las clasificamos así porque son las funciones que componen al CPU para que haga su trabajo.

void				handshake(int socket, u_int32_t codigoCPU);											// Base para todos los handshakes.

solicitudMemoria 	generarSolicitudMemoria(pcb* unPcb, u_int32_t tamanioPaginas);						// Generar una solicitud de datos a memoria.
// TODO: Testear
u_int32_t			hallarPagina(u_int32_t posicionInstruccion, u_int32_t tamanioPaginas);				// Actualiza estructuras tras una operación.
// TODO: Testear
void 				interpretarOperacion(); 															// Recibe una instrucción de un programa y la parsea.

void 				ejecutarOperacion(); 																// Ejecuta una instrucción parseada.

void 				llamarFuncion(/* Stack */); 														// Llama a una función o procedimiento.

void 				actualizarPC(int *PC, u_int32_t valor); 											// Incrementa el Program Counter con la próxima instrucción a ejecutar.

void 				arrojarExcepcion(/* Excepción */); 													// Se explica solo.



// Definiciones

void handshakeKernel(int socketKernel, u_int32_t codigoCPU){
	handshakeKernel(socketKernel, codigoCPU);
}

void handshakeMemoria(int socketMemoria, u_int32_t codigoCPU, u_int32_t* tamanioPaginas){
	handshake(socketMemoria, codigoCPU);
	recibirDatos(socketMemoria, tamanioPaginas, sizeof(u_int32_t));
}

char* fetchInstruction(int socketMemoria, solicitudMemoria instruccion){
	char* instruccionSolicitada;

	// TODO: Envío de solicitud de instrucción a memoria.

	// TODO: Recibir solicitud de instrucción de memoria.

	return instruccionSolicitada;
}

solicitudMemoria generarSolicitudMemoria(pcb* unPcb, u_int32_t tamanioPaginas){
	solicitudMemoria solicitudInstruccion;

	int proximaInstruccion = unPcb->programCounter; 								// Indica cuál es la proxima instrucción a ejecutar.

	lineaUtil instruccion = unPcb->indiceCodigo[proximaInstruccion];				// Obtengo el la posición de inicio y fin del programa.
	u_int32_t posicionInstruccion = proximaInstruccion * 2 * sizeof(u_int32_t); 	// Obtengo la posición de la instrucción en bytes.

	u_int32_t pagina = hallarPagina(posicionInstruccion, tamanioPaginas); 			// Esta es la página en Memoria donde está la instrucción.
	u_int32_t offset = instruccion.offset; 											// Esto es el desplazamiento en bytes del código respecto al inicio del programa escrito.
	u_int32_t longitud = instruccion.longitud; 										// Esto es la longitud en bytes de la instrucción respecto del offset.

	solicitudInstruccion.pid = unPcb->pid;											// Creación de la estructura para enviar solicitud.
	solicitudInstruccion.pagina = pagina;
	solicitudInstruccion.offset = offset;
	solicitudInstruccion.size = longitud;

	return solicitudInstruccion;
}

u_int32_t hallarPagina(u_int32_t posicionInstruccion, u_int32_t tamanioPaginas){
	u_int32_t pagina = 0;

	while(posicionInstruccion > (pagina + 1) * tamanioPaginas)
		pagina++;

	return pagina;
}

void handshake(int socket, u_int32_t codigoCPU){
	int bytesRecibidos;

	char datosEnviar[1024], datosRecibir[1024];
	memset(datosEnviar, '\0', 1024);
	memset(datosRecibir, '\0', 1024);

	send(socket, &codigoCPU, sizeof(u_int32_t), 0);
	bytesRecibidos = recv(socket, datosRecibir, 1024, 0);

	datosRecibir[bytesRecibidos] = '\0';
}

void actualizarPC(int *PC, u_int32_t valor){
	*PC = valor;
}

#endif /* FUNCIONESCPU_H_ */
