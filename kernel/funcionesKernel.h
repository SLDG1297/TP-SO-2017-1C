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

#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/controlErrores.h"
#include "../librerias/pcb.h"
#include "../librerias/conexionSocket.h"


#define RUTA_ARCHIVO "./config_kernel.cfg"

//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION

	int	PUERTO_KERNEL;
	int PUERTO_CPU;
	int PUERTO_MEMORIA;
	int PUERTO_FS;
	char* IP_MEMORIA;
	char* IP_FS;

	int QUANTUM;
	int QUANTUM_SLEEP;
	int GRADO_MULTIPROG;
	int SEM_INIT [3];
	int STACK_SIZE;

	char* ALGORITMO;
	char* SEM_IDS [3];
	char* SHARED_VARS[3];
// DECLARACION MEMORIA
	int frameSize;


void iniciarConfiguraciones (){
	//CODIGO PARA LLAMAR AL ARCHIVO

	//Estructura para manejar el archivo de configuración -- t_config*
	//Crear estructura de configuración para obtener los datos del archivo de configuración.
	char* ruta = RUTA_ARCHIVO;
	t_config* configuracion = llamarArchivo(ruta);


	PUERTO_KERNEL = busquedaClaveNumerica(configuracion,"PUERTO_KERNEL");
	PUERTO_CPU = busquedaClaveNumerica(configuracion,"PUERTO_CPU");
	PUERTO_MEMORIA = busquedaClaveNumerica(configuracion,"PUERTO_MEMORIA");
	PUERTO_FS = busquedaClaveNumerica(configuracion,"PUERTO_FS");
	IP_MEMORIA = busquedaClaveAlfanumerica(configuracion,"IP_MEMORIA");
 	IP_FS = busquedaClaveAlfanumerica(configuracion,"IP_FS");

 	QUANTUM = busquedaClaveNumerica(configuracion,"QUANTUM");
 	QUANTUM_SLEEP = busquedaClaveNumerica(configuracion,"QUANTUM_SLEEP");
 	GRADO_MULTIPROG= busquedaClaveNumerica(configuracion,"GRADO_MULTIPROG");
 	SEM_INIT [3];
 	STACK_SIZE= busquedaClaveNumerica(configuracion,"STACK_SIZE");

 	ALGORITMO = busquedaClaveAlfanumerica(configuracion, "ALGORITMO");
 	SEM_IDS [3];
 	SHARED_VARS[3];
}


int asignarSocketMemoria(){

		int sockMemoria, rtaFuncion;
		struct sockaddr_in memoria_dir;

		sockMemoria = conectar(IP_MEMORIA,PUERTO_MEMORIA);

		return sockMemoria;
}

int asignarSocketFS(){
	int sockFS;

  sockFS = conectar(IP_FS,PUERTO_FS);

	return sockFS;
}

int asignarSocketListener(){
	int socketKernel, rtaFuncion;
  struct sockaddr_in kernel;

  socketKernel=servir(PUERTO_KERNEL);

	printf("\nEsperando en el puerto %i\n", PUERTO_KERNEL);
  return socketKernel;
}

int obtenerTamanioPagina(int sock){
  int rtaFuncion;
  int tamanioDePagina;
  rtaFuncion= recv (sock,tamanioDePagina, sizeof(tamanioDePagina),0);
  esErrorConSalida(rtaFuncion,"Error al obetener el tamaño de pagina");
  return tamanioDePagina;
  }

void  handshake(int sock){
	int rtaFuncion;
  int handshake=1;
  	rtaFuncion = send(sock,handshake,sizeof(handshake),0);
    esErrorConSalida(rtaFuncion,"Error en el handshake de memoria (envio)");
  }

void setFrameSize(int tamanio){
		frameSize=tamanio;
}

void generarMenu(void){
	//Borrarpantalla clear??
	printf ("consola del kernel\n");
	printf ("1-Listado de procesos en el sistema\n");
	printf ("2-Acciones para un proceso determinado\n");
	printf ("3-Tabla global de archivos\n");
	printf ("4- Modificar grado de multiprogramacion\n");
	printf ("5-Finalizar un proceso\n");
	printf ("6-Detener la planificacion\n");
	}

void generarMenuDeProceso(void){
		printf ("Elija una accion\n");
		printf ("1- Cantidad de rafagas ejecutadas\n");
		printf ("2-Cantidad de operaciones privilegiadas ejecutadas\n");
		printf ("3-Tabla de archivos abiertos por el proceso\n");
		printf ("4-Cantidad de paginas del heap utilizadas\n");
		printf ("5-Cantidad de Syscalls ejecutadas\n");
		}

void generarMenuDeHeap(void){
	printf ("1- cantidad de acciones alojar realizadas\n");
	printf ("2- cantidad de acciones liberar realizadas\n");
}


void obtenerOrden(int* orden){
		printf("Elija una opcion\n");
		scanf("%d",orden);
		}

void incrementarcontadorPid (contadorPid){
	contadorPid++;
}

void mensajeDeError (int* orden){
	printf("%d no es una orden valida\n", orden);
}


#endif /* FUNCIONESKERNEL_H_ */

