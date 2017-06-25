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
#include <stdbool.h>

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
#include "../pcb.h"
#include "../conexionSocket.h"

#define ID_CONSOLA	1
#define ID_CPU		2
#define ID_FS 		3

// DECLARACION DE TIPOS
typedef struct {
	int socketcpu;
	pcb pcbAsociado;
} cpuAsociadoAPcb;

typedef struct {
	bool activo;
	int socket;
} consola_activa;

typedef struct {
	bool activo;
	int socket;
} cpu_activo;

//DECLARACION DE LISTAS

t_list *procesosActivos;
t_list *consolas;
t_list *cpus;
t_list *pcbs;

//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION
int _puertoKernel;
int _puertoCpu;
int _puertoMemoria;
int _puertoFS;
char* _ipMemoria;
char* _ipFs;

int _quantum;
int _quantumSleep;
int _gradoMultiprog;
int _semInit[3];
int _stackSize;

char* _algoritmo;
char* _semIds[3];
char* sharedVars[3];
// DECLARACION MEMORIA
int frameSize;

int socketPosible;

//DECLARACION DE FUNCIONES
void iniciarConfiguraciones(char* ruta);
int asignarSocketFS();
int asignarSocketMemoria();
int asignarSocketListener();
void agregarALista(int tipo, int socketDato, t_list *lista);
void *consolaOperaciones();
void operacionesParaProceso();
int obtenerTamanioPagina(int sock);
void handshake(int sock);
void setFrameSize(int tamanio);
int obtenerOrden();
void incrementarContadorPid(int *contadorPid);
void mensajeDeError(int orden);
bool perteneceAListaCpu(int socketPosible, cpu_activo nodoCpu);
bool perteneceAListaConsola(consola_activa* consola);

void iniciarConfiguraciones(char* ruta) {
	//CODIGO PARA LLAMAR AL ARCHIVO

	//Estructura para manejar el archivo de configuración -- t_config*
	//Crear estructura de configuración para obtener los datos del archivo de configuración.

	t_config* configuracion = llamarArchivo(ruta);

	_puertoKernel = busquedaClaveNumerica(configuracion, "PUERTO_KERNEL");
	_puertoCpu = busquedaClaveNumerica(configuracion, "PUERTO_CPU");
	_puertoMemoria = busquedaClaveNumerica(configuracion, "PUERTO_MEMORIA");
	_puertoFS = busquedaClaveNumerica(configuracion, "PUERTO_FS");
	_ipMemoria = busquedaClaveAlfanumerica(configuracion, "IP_MEMORIA");
	_ipFs = busquedaClaveAlfanumerica(configuracion, "IP_FS");

	_quantum = busquedaClaveNumerica(configuracion, "QUANTUM");
	_quantumSleep = busquedaClaveNumerica(configuracion, "QUANTUM_SLEEP");
	_gradoMultiprog = busquedaClaveNumerica(configuracion, "GRADO_MULTIPROG");
	_semInit[3];
	_stackSize = busquedaClaveNumerica(configuracion, "STACK_SIZE");

	_algoritmo = busquedaClaveAlfanumerica(configuracion, "ALGORITMO");
	_semIds[3];
	sharedVars[3];

}

// ************** ASIGNACION SOCKETS ***********
int asignarSocketMemoria() {

	return conectar(_ipMemoria, _puertoMemoria);
}

int asignarSocketFS() {
	return conectar(_ipFs, _puertoFS);

}

int asignarSocketListener() {
	int socketKernel;

	socketKernel = servir(_puertoKernel);

	printf("\nEsperando en el puerto %i\n", _puertoKernel);
	return socketKernel;
}
// ************** INICIALIZAR LISTAS ***********
void inicializarListas() {
	procesosActivos = list_create();
	consolas = list_create();
	cpus = list_create();
	pcbs = list_create();

}

void agregarALista(int tipo, int socketDato) {
	consola_activa consola;
	cpu_activo cpu;
	switch (tipo) {
	case 0:
		//si tipo igual a cero, se crea una consola
		break;
	case 1:

		consola.socket = socketDato;
		consola.activo = false;
		list_add(consolas, &consola);
		break;
	case 2:

		cpu.socket = socketDato;
		cpu.activo = false;
		list_add(cpus, &cpu);
		break;
	default:
		printf("Error de tipo");
		break;

	}

}
void *consolaOperaciones() {
	printf("Consola de kernel\n");

	generarMenu();
	int ordenDeConsola = obtenerOrden();
	switch (ordenDeConsola) {
	case 1:
		
		//mostrar listado de procesosActivos del sistema\n
		break;
	case 2:
		operacionesParaProceso();
		break;
	case 3:
		//mostrar tabla global de archivos
		break;
	case 4:
		printf("Ingrese el nuevo grado de multiprogramacion");
		scanf("%d",_gradoMultiprog);
		printf("El nuevo grado de multiprogramacion es %d",_gradoMultiprog);

		break;
	case 5:
		//finalizar un proceso
		break;
	case 6:
		//detener la planificacion
		break;
	default:
		mensajeDeError(ordenDeConsola);
		break;
	}
	return 0;

}

void operacionesParaProceso() {
	int opcion;
	generarMenuDeProceso();
	opcion = obtenerOrden();
	switch (opcion) {
	case 1:
		//mostrar cantidad de rafagas ejecutadas
		break;
	case 2:
		//mostrar cantidad de operaciones privilegiadas ejecutadas
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
void realizarOperacionDeSocket(int socketPosible) {
	int modulo = determinarTipoSocket(socketPosible);
	switch (modulo) {
	case ID_CPU:
		//pcbRecibido = recibirYDeserializarPCB();
		//actualizarPCB(pcbRecibido);

		break;
	case ID_CONSOLA:
		char* contenido;
		//Si no es listener, es consola, cpu, o filesystem

		//Si es Consola

		//recibir instruccion

		//Nuevo Programa

		//recibir path

		pcb nuevoProceso;

		preprocesador(contenido, &nuevoProceso);
		//generarLineasUtiles(Path)

		//creando pcb

		//pcb nuevoPCB;

		//nuevoPCB.pid = contadorPid;

		//incrementarcontadorPid();

		//nuevoPCB.programCounter = 0;

		//tamañoPath = calcularTamañoPath(Path);

		//nuevoPCB.paginasUsadas = tamañoPath / tamañoPagina;

		//nuevoPCB.indiceCodigo =

		//nuevoPCB.indiceEtiqueta=

		//nuevoPCB.indiceStack=

		//nuevoPCB.exitCode=

		/*		 //solicitar memoria

		 //send(sockMemoria, &nuevoPCB.pid, sizeof(int), 0);

		 //send(sockMemoria, &nuevoPCB.paginasUsadas, sizeof(int), 0);

		 //recv(sockMemoria, &validacionDeMemoria, sizeof(int), 0);



		 //enviar pcb mas codigo a memoria



		 //elegir cpu_activa

		 //enviar a cpu_activa



		 //terminar programa

		 break;
		 case ID_FS:
		 break;
		 default:
		 break;*/

	}

}

void actualizarPCB(pcb pcbRecibido) {
	//void (*constante) (void*)

}

int buscarIndice(cpu_activo *elemento, t_list *lista) {
	int tamanio = list_size(lista);
	int indice = 0;

	while (indice < tamanio) {
		cpu_activo *nodo = list_get(lista, indice);
		if (nodo != NULL) {
			if (elemento->activo == nodo->activo
					&& elemento->socket == nodo->socket)
				return indice;
			indice++;
		} else
			return -1;

	}
	return -1;
}

int buscarIndice(consola_activa *elemento, t_list *lista) {
	int tamanio = list_size(lista);
	int indice = 0;

	while (indice < tamanio) {
		cpu_activo *nodo = list_get(lista, indice);
		if (elemento->activo == nodo->activo
				&& elemento->socket == nodo->socket)
			return indice;
		indice++;
	}
	return -1;
}

int determinarTipoSocket(int sp) {
	socketPosible = sp;
	bool (*condicionCpu)(void*) = &perteneceAListaCpu;
	bool (*condicionConsola)(void*) = &perteneceAListaConsola;

	//bool list_any_satisfy(t_list* self, bool(*condition)(void*));
	if (list_any_satisfy(cpus, condicionCpu))
		return ID_CPU; // cpu
	if (list_any_satisfy(consolas, condicionConsola))
		return ID_CONSOLA;

	return ID_FS;
}

bool perteneceAListaCpu(cpu_activo* cpu) {
	return (cpu->socket == socketPosible);
}

bool perteneceAListaConsola(consola_activa* consola) {
	return (consola->socket == socketPosible);
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
	int orden = 0;
	printf("Elija una opcion\n");
	scanf("%d", orden);
	return orden;
}
void incrementarContadorPid(int* contadorPid) {
	*contadorPid++;
}

void mensajeDeError(int orden) {
	printf("%d no es una orden valida\n", orden);
}

#endif /* FUNCIONESKERNEL_H_ */

