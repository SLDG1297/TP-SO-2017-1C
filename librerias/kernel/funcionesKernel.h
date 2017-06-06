/*
 * funcionesKernel.h
 *
 *  Created on: 5/6/2017
 *      Author: utnso
 */

#ifndef FUNCIONESKERNEL_H_
#define FUNCIONESKERNEL_H_

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/bitarray.h>
#include <commons/temporal.h>
#include <commons/process.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <commons/collections/node.h>

#include <parser/metadata_program.h>

#include "../controlArchivosDeConfiguracion.h"
#include "../controlErrores.h"
#include "../cpu/pcb.h"
#include "../conexionSocket.h"

#define RUTA_ARCHIVO "../../kernel/config_kernel.cfg"

//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION

int PUERTO_KERNEL;
int PUERTO_CPU;
int PUERTO_MEMORIA;
int PUERTO_FS;
char* IP_MEMORIA;
char* IP_FS;

int QUANTUM;
int QUANTUM_SLEEP;
int GRADO_MULTIPROG;
int SEM_INIT[3];
int STACK_SIZE;

char* ALGORITMO;
char* SEM_IDS[3];
char* SHARED_VARS[3];
// DECLARACION MEMORIA
int frameSize;

//DECLARACION DE FUNCIONES
void handshake(int sock);
void consolaKernel();
int obtenerOrden();
void operacionesParaProceso();

// DECLARACION DE TIPOS
typedef struct {
	int socketcpu;
	pcb pcbAsociado;
} cpuAsociadoAPcb;

typedef struct {
	int estado;
	int socket;
} consola_activa;

typedef struct {
	int estado;
	int socket;
} cpu_activo;


void iniciarConfiguraciones() {
	//CODIGO PARA LLAMAR AL ARCHIVO

	//Estructura para manejar el archivo de configuración -- t_config*
	//Crear estructura de configuración para obtener los datos del archivo de configuración.
	char* ruta = RUTA_ARCHIVO;
	t_config* configuracion = llamarArchivo(ruta);

	PUERTO_KERNEL = busquedaClaveNumerica(configuracion, "PUERTO_KERNEL");
	PUERTO_CPU = busquedaClaveNumerica(configuracion, "PUERTO_CPU");
	PUERTO_MEMORIA = busquedaClaveNumerica(configuracion, "PUERTO_MEMORIA");
	PUERTO_FS = busquedaClaveNumerica(configuracion, "PUERTO_FS");
	IP_MEMORIA = busquedaClaveAlfanumerica(configuracion, "IP_MEMORIA");
	IP_FS = busquedaClaveAlfanumerica(configuracion, "IP_FS");

	QUANTUM = busquedaClaveNumerica(configuracion, "QUANTUM");
	QUANTUM_SLEEP = busquedaClaveNumerica(configuracion, "QUANTUM_SLEEP");
	GRADO_MULTIPROG = busquedaClaveNumerica(configuracion, "GRADO_MULTIPROG");
	SEM_INIT[3];
	STACK_SIZE = busquedaClaveNumerica(configuracion, "STACK_SIZE");

	ALGORITMO = busquedaClaveAlfanumerica(configuracion, "ALGORITMO");
	SEM_IDS[3];
	SHARED_VARS[3];
}
// ************** ASIGNACION SOCKETS ***********
int asignarSocketMemoria() {

	int sockMemoria;

	sockMemoria = conectar(IP_MEMORIA, PUERTO_MEMORIA);

	return sockMemoria;
}

int asignarSocketFS() {
	int sockFS;

	sockFS = conectar(IP_FS, PUERTO_FS);

	return sockFS;
}

int asignarSocketListener() {
	int socketKernel;

	socketKernel = servir(PUERTO_KERNEL);

	printf("\nEsperando en el puerto %i\n", PUERTO_KERNEL);
	return socketKernel;
}

void agregarALista(int tipo, int socketDato, t_list *lista) {
	consola_activa consola;
	cpu_activo cpu;
	switch (tipo){
	case 0:
		//si tipo igual a cero, se crea una consola
		break;
	case 1:

		consola.socket = socketDato;
		lista_add(lista,&consola);
		break;
	case 2:

		cpu.socket = socketDato;
		list_add(lista, &cpu);
		break;
	default:
		printf("Error de tipo");
		break;


	}

}
void consolaKernel() {


	//hilo de interfaz de consola
	generarMenu();

	switch (obtenerOrden()) {
		case 1:
								//mostrar listado de procesos del sistema\n
		case 2:
			operacionesParaProceso();
		case 3:
								//mostrar tabla global de archivos
			break;
		case 4:
								//modificar grado de multiprogramacion
			break;
		case 5:
								//finalizar un proceso
			break;
		case 6:
								//detener la planificacion
			break;
		default:
			mensajeDeError(&ordenDeConsola);
			break;
	}

}

void operacionesParaProceso(){
	int opcion;
	generarMenuDeProceso();
	opcion=obtenerOrden();
				switch (opcion) {
					case 1:
											//mostrar cantidad de rafagas ejecutadas
						break;
					case 2:
										//mostrar cantidad de operaciones provilegiadas ejecutadas
						break;
					case 3:
											//obtener la tabla de archivos abiertos
						break;
					case 4:
											//cantidad de paginas de heap
						generarMenuDeHeap();

							if (obtenerOrden() == 1) {
											//mostrar cantidad de acciones alojar ejecutadas por el proceso
							} else {
											//mostrar cantidad de acciones Liberar ejecutadas por el proceso
							}
						break;
					case 5:
											//mostrar cantidad de syscalls ejecutadas
						break;
					default:
						break;
				}
}



int obtenerTamanioPagina(int sock) {
	int rtaFuncion;
	int tamanioDePagina;
	rtaFuncion = recv(sock, &tamanioDePagina, sizeof(tamanioDePagina), 0);
	esErrorConSalida(rtaFuncion, "Error al obetener el tamaño de pagina");
	return tamanioDePagina;
}

void handshake(int sock) {
	int rtaFuncion;
	int handshake = 1;
	rtaFuncion = send(sock, &handshake, sizeof(handshake), 0);
	esErrorConSalida(rtaFuncion, "Error en el handshake de memoria (envio)");
}

void setFrameSize(int tamanio) {
	frameSize = tamanio;
}


int obtenerOrden() {
	int orden;
	printf("Elija una opcion\n");
	scanf("%d", orden);
	return orden;
}

void incrementarcontadorPid( int contadorPid) {
	contadorPid++;
}

void mensajeDeError(int* orden) {
	printf("%d no es una orden valida\n", *orden);
}

#endif /* FUNCIONESKERNEL_H_ */

