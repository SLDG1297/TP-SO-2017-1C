/*
 ============================================================================
 Name        : memoria.c
 Author      : Zero Gravity
 ============================================================================
 */

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
#include <pthread.h>

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

#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/controlErrores.h"
#include "../librerias/funcionesMemoria.h"
#include "../librerias/operacionesMemoria.h"

#define RUTA_ARCHIVO "./config_memoria.cfg"
#define SIZE_DATA 1024
#define RUTA_LOG "./memoriaLog.txt"

//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

//*Socket para esperar conexion con el kernel
int socketEsperaKernel;
int bytesRecibidos;


//*Socket para entablar conexion con el kernel
int sockAlKernel;
int yes = 1;
struct sockaddr_in espera, datosDelKernel;

//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION

int puertoMemoria;
int frameSize;
int frames;
int entradasCache;
int cache_x_proc;
int retardoMemoria;

//DECLARACION DE PROTOTIPO DE FUNCIONES


int crearSocket();
void laConexionFueExitosa(char* handshake);
void iniciarConexion();


//************** MAIN **************//
int main(int argc, char *argv[]) {

//DECLARACION DE VARIABLES PARA EL CODIGO PRINCIPAL


//*Socket para entablar conexion con el kernel
	char buffer[SIZE_DATA];
	char* handshake = "La conexion al proceso memoria fue exitosa";
	memset(buffer, '\0', SIZE_DATA);

	//*Puntero a memoria contigua
	int *ptrMemoria;


// CODIGO PRINCIPAL DE LA CONSOLA
	//*solicitud de bloque de memoria contigua
	asignarDatosDeConfiguracion(&puertoMemoria, &frameSize, &frames,
			&entradasCache, &cache_x_proc, &retardoMemoria);

	ptrMemoria = malloc(frameSize * frames);
	if (ptrMemoria == NULL)
		esErrorConSalida(-1,
				"Error en la solicitud de bloque de memoria (Malloc)");
	crearArchivo(RUTA_LOG);
	inicializarStrAdm(frames, frameSize,&ptrMemoria);
	//*asignacion de datos al socket que espera a la conexion del kernel

	//DIRECCION PARA ESPERAR A LA CONEXION CON EL KERNEL
	iniciarConexion();

	laConexionFueExitosa(handshake);


	puts("La conexion al proceso kernel fue exitosa\n");
	puts("Esperando mensajes\n");

//	while (1) {
//
//		bytesRecibidos = recv(sockAlKernel, buffer, sizeof(buffer), 0);
//		esErrorSinSalida(bytesRecibidos, "Fallo en la recepcion");
//
//		sinBytesRecibidos(bytesRecibidos);
//
//		printf("Buffer: %s\n", buffer);
//		memset(buffer, '\0', SIZE_DATA);
//	}


	return 0;

}

void iniciarConexion() {


	socketEsperaKernel = crearSocket();
	printf("Setsockopt correcto\n");
	puts("Socket de espera creado\n");

	//Declaracion de variables para recibir valor de rta
	int valorRtaBind = 0;
	int valorRtaListen = 0;

	int longitudEstructuraSocket = sizeof(datosDelKernel);
	//DIRECCION PARA ESPERAR A LA CONEXION CON EL KERNEL


	espera.sin_family = AF_INET;
	espera.sin_port = htons(puertoMemoria);
	espera.sin_addr.s_addr = INADDR_ANY;
	bzero(&(espera.sin_zero), 8);

//Bind(): Asigna la direccion de addr al socket referido por el SocketEsperaKernel
	valorRtaBind = bind(socketEsperaKernel, (struct sockaddr *) &espera,
			sizeof(struct sockaddr));
	errorSalidaSocket(valorRtaBind, "Error en Bind",&socketEsperaKernel);

	printf("Bind correcto\n");
	puts("Esperando la conexion del kernel\n");

//Listen(): Marca al socket como pasivo, es decir un socket que sera utilizado para aceptar conecciones
	valorRtaListen = listen(socketEsperaKernel, 5);
	errorSalidaSocket(valorRtaListen, "Error en Listen",&socketEsperaKernel);

//Accept(): Extrae el primer pedido de conexion de la cola de conexiones pendientes, del socket de escucha
//crea un nuevo socket conecatado y retorna un nuevo descriptor de archivo referido a ese socket.
	sockAlKernel = accept(socketEsperaKernel, (struct sockaddr *) &datosDelKernel,
			&longitudEstructuraSocket);
	errorSalidaSocket(sockAlKernel, "Error en el accept",&socketEsperaKernel);
}
int crearSocket(){
	//Declaracion de variable para recibir valor de rta
	int valorRtaSetSockOpt;
	int sockDeEspera;

	//Socket(): creates an endpoint for communication and returns a file descriptor that refers to that endpoint.

	sockDeEspera =socket(AF_INET, SOCK_STREAM, 0);
	errorSalidaSocket(sockDeEspera, "Fallo en la creacion del socket de espera",&sockDeEspera);

	valorRtaSetSockOpt = setsockopt(sockDeEspera, SOL_SOCKET, SO_REUSEADDR,
				(const char*)&yes, sizeof(yes));
	errorSalidaSocket(valorRtaSetSockOpt,
				"Error en setSockOpt en el socket de espera",&sockDeEspera);
	return sockDeEspera;
}
void laConexionFueExitosa(char* handshake){
	int longitudDatosEnviados;
	longitudDatosEnviados = send(sockAlKernel, handshake, strlen(handshake), 0);
	esErrorSinSalida(longitudDatosEnviados, "Fallo en el handshake");

}
