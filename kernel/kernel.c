/*
 ============================================================================
 Name        : kernel.c
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
#include "../librerias/cpu/pcb.h"

#include "../librerias/kernel/funcionesKernel.h"
#include "../librerias/kernel/menuContextualKernel.h"

#define RUTA_ARCHIVO "./config_kernel.cfg"
#define SIZE_DATA 1024
#define IDCONSOLA 1
#define IDCPU 2
#define INICIAR_PROGRAMA 51
#define SOLICITAR_BYTES_PAG 52
#define ALMACENAR_BYTES_PAG 53
#define ASIGNAR_PAGINAS_PRC 54
#define FINALIZAR_PRG 55

int ordenDeConsolaParaProceso;
int ordenDeHeap;
int identificador;
int contadorPid = 1;



// ************** MAIN *********************
int main(int argc, char *argv[]) {

//DECLARACION DE VARIABLES PARA EL CODIGO PRINCIPAL

//*Lista completa de sockets
	fd_set socketsRelevantes;
	//*Sockets filtrados por el select
	fd_set socketsFiltrados;
	FD_ZERO(&socketsRelevantes);
	FD_ZERO(&socketsFiltrados);
	//*Numero del descriptor de fichero mas grande
	int fileDescMax;
	//*Socket para escuchar nuevas conexiones
	int sockMemoria;
	int sockListener;
	int sockFS;
	//*Socket para aceptar
	int nuevoSocket;
	int longitudBytesRecibidos;
	int yes = 1;
	int longitudEstructuraSocket;
	//*Contadores del for
	int i, j;
	int validacionDeMemoria = 0;


	struct timeval tv;

	t_list *procesos = list_create();
	t_list *consolas = list_create();
	t_list *cpus = list_create();
	t_list *pcbs = list_create();

	typedef struct {
		int pid;
		int rafagasEjecutadas;
		int syscallsEjecutadas;

	} informacionDeProceso;

	iniciarConfiguraciones();

//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

	int valorRtaRecv = 0;
	int valorRtaSelect = 0;

//CONEXION AL PROCESO MEMORIA

	sockMemoria = asignarSocketMemoria();
	handshake(sockMemoria);
	setFramesize(obtenerTamanioPagina(sockMemoria));

	FD_SET(sockMemoria, &socketsRelevantes);

//CONEXION AL PROCESO FILE SYSTEM

	sockFS = asignarSocketFS();
	handshake(sockFS);

	FD_SET(sockFS, &socketsRelevantes);

//CREACION DEL SOCKET ESCUCHA Y VERIFICACION DE ERROR

	sockListener = asignarSockeListener();

	//*Se agregan el listener a los sockets relevantes y se lo asigna como maximo por ser el unico en la lista en este momento
	FD_SET(sockListener, &socketsRelevantes);
	fileDescMax = sockListener;

	printf("Consola de kernel\n");
	pthread_t idHilo;
	pthread_create(&idHilo, NULL, consolaKernel, NULL);
	char* buffer;
//hilo de escucha 
	while (1) {
		memset(buffer, '\0', SIZE_DATA);
		socketsFiltrados = socketsRelevantes;

		valorRtaSelect = select(fileDescMax + 1, &socketsFiltrados, NULL, NULL,
				&tv);
		esErrorConSalida(valorRtaSelect, "Error en select");

		//int nroConsola=0;
		for (i = 0; i <= fileDescMax; i++) {

			if (FD_ISSET(i, &socketsFiltrados)) {

				if (i == sockListener) {

					longitudEstructuraSocket = sizeof(cliente_dir);
					nuevoSocket = accept(sockListener,
							(struct sockaddr *) &cliente_dir,
							&longitudEstructuraSocket);
					esErrorConSalida(nuevoSocket, "error en accept");
					valorRtaRecv = recv(nuevoSocket, identificador, sizeof(int),
							0);
					esErrorConSalida(valorRtaRecv, "Error en recv ID");

					switch (identificador) {
					case IDCONSOLA:
						//Si la conexion es una consola, agregamos el socket a relavantes y enviamos mensaje de aceptacion.
						FD_SET(nuevoSocket, &socketsRelevantes);
						send(nuevoSocket,
								"Conexion aceptada. Bienvenido, proceso Consola",
								strlen(
										"Conexion aceptada. Bienvenido, proceso Consola"),
								0);

						//Actualizando el maximo descriptor de fichero.
						if (nuevoSocket > fileDescMax)
							fileDescMax = nuevoSocket;

						agregarALista(IDCONSOLA, nuevoSocket, consolas);
						//mensaje de nueva conexion de consola.
						printf(
								"%s: Nueva conexion de una consola, ip:%s en el socket %d\n",argv[0], inet_ntoa(cliente_dir.sin_addr),nuevoSocket);
						break;

					case IDCPU:
						//Si la conexion es una cpu, agregamos el socket a relavantes y enviamos mensaje de aceptacion.
						FD_SET(nuevoSocket, &socketsRelevantes);
						send(nuevoSocket,
								"Conexion aceptada. Bienvenido, proceso Cpu",
								strlen(
										"Conexion aceptada. Bienvenido, proceso Cpu"),
								0);

						//Actualizando el maximo descriptor de fichero.
						if (nuevoSocket > fileDescMax)
							fileDescMax = nuevoSocket;
						agregarALista(IDCPU, nuevoSocket, cpus);
						//mensaje de nueva conexion de cpu.
						printf(
								"%s: Nueva conexion de un cpu, ip:%s en el socket %d\n",argv[0], inet_ntoa(cliente_dir.sin_addr),nuevoSocket);
						break;
					default:
						printf("Identificación incorrecta");
						break;
					}

				} else {
					/*//Si no es listener, es consola, cpu, o filesystem

					//Si es CPU:
					//recibir pcb
					//actualizar pcb
					//

					//Si es Consola
					//recibir instruccion
					//Nuevo Programa
					//recibir path

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

					//solicitar memoria
					//send(sockMemoria, &nuevoPCB.pid, sizeof(int), 0);
					//send(sockMemoria, &nuevoPCB.paginasUsadas, sizeof(int), 0);
					//recv(sockMemoria, &validacionDeMemoria, sizeof(int), 0);

					//enviar pcb mas codigo a memoria

					//elegir cpu_activa
					//enviar a cpu_activa

					//terminar programa
*/
								}

							}

						}						//cierra - if(FD_ISSET(j, &socketsRelevantes))

					}						//cierra - for(j = 0; j <= fileDescMax; j++)

				}						//cierra - else puts(buffer)


pthread_join(&idHilo, NULL);
return 0;
}

//Agregas un elemento a la lista segun el tipo




/*
 bool compararSockets(int socket1, int socket2){
 return socket1==socket2 ? true : false
 }*/

