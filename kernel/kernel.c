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
#include <pthread.h>

#include <parser/metadata_program.h>

#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/controlErrores.h"
#include "../librerias/pcb.h"

#include "../librerias/kernel/funcionesKernel.h"
#include "../librerias/kernel/menuContextualKernel.h"

#define RUTA_ARCHIVO "./config_kernel.cfg"
#define SIZE_DATA			1024
#define ID_CONSOLA			1
#define ID_CPU				2
#define ID_FS				3
#define INICIAR_PROGRAMA 	51
#define SOLICITAR_BYTES_PAG 52
#define ALMACENAR_BYTES_PAG 53
#define ASIGNAR_PAGINAS_PRC 54
#define FINALIZAR_PRG 		55

int ordenDeConsolaParaProceso;
int ordenDeHeap;
int contadorPid = 1;


// **************** DECLARACION DE VARIABLES PARA EL CODIGO PRINCIPAL ****************

// LISTA COMPLETA DE SOCKETS
	fd_set socketsRelevantes;

// SOCKETS FILTRADOS POR EL SELECT
	fd_set socketsFiltrados;

// NRO DEL DESCRIPTOR DE FICHERO MAS GRANDE
	int fileDescMax;

// SOCKET PARA NUEVAS CONEXIONES
	int sockMemoria;
	int sockListener;
	int sockFS;

//SOCKET PARA ACEPTAR
	int nuevoSocket;
	int longitudBytesRecibidos;
	int yes = 1;
	int longitudEstructuraSocket;

// CONTADORES FOR
	int socketPosible, j;
	int validacionDeMemoria = 0;
	struct sockaddr_in cliente_dir;



	typedef struct {
		int pid;
		int rafagasEjecutadas;
		int syscallsEjecutadas;

	} informacionDeProceso;

//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

		int valorRtaRecv = 0;
		int valorRtaSelect = 0;

//DECLARACION DE PROTOTIPOS DE FUNCIONES
		void operacionSegunIdentificador(int identificador);
// ************** MAIN *********************
int main(int argc, char *argv[]) {



	iniciarConfiguraciones(RUTA_ARCHIVO);
	inicializarListas();

	FD_ZERO(&socketsRelevantes);
	FD_ZERO(&socketsFiltrados);

//CONEXION AL PROCESO MEMORIA

	sockMemoria = asignarSocketMemoria();
	handshake(sockMemoria);
	setFrameSize(obtenerTamanioPagina(sockMemoria));

	FD_SET(sockMemoria, &socketsRelevantes);

//CONEXION AL PROCESO FILE SYSTEM

	sockFS = asignarSocketFS();
	handshake(sockFS);

	FD_SET(sockFS, &socketsRelevantes);

//CREACION DEL SOCKET ESCUCHA Y VERIFICACION DE ERROR

	sockListener = asignarSocketListener();

	//*Se agregan el listener a los sockets relevantes y se lo asigna como maximo por ser el unico en la lista en este momento
	FD_SET(sockListener, &socketsRelevantes);
	fileDescMax = sockListener;

	printf("Consola de kernel\n");
	pthread_t idHilo;
	pthread_create(&idHilo, NULL, consolaOperaciones, NULL);


//hilo de escucha
	while (1) {

		socketsFiltrados = socketsRelevantes;
		valorRtaSelect = select(fileDescMax + 1, &socketsFiltrados, NULL, NULL,
				NULL);
		esErrorConSalida(valorRtaSelect, "Error en select");

		//int nroConsola=0;

		for (socketPosible = 0; socketPosible <= fileDescMax; socketPosible++) {

			if (FD_ISSET(socketPosible, &socketsFiltrados)) {

				if (socketPosible == sockListener) {

					realizarOperacionDeListener(nuevoSocket);

				} // cierra if
				else {
					realizarOperacionDeSocket(socketPosible);

				} //cierra Else

			}					//cierra - if(FD_ISSET(j, &socketsRelevantes))

		}						// cierra for for

	}	// cierra while

	pthread_join(idHilo, NULL);

	return 0;

}

//Agregas un elemento a la lista segun el tipo

/*
 bool compararSockets(int socket1, int socket2){
 return socket1==socket2 ? 1 : 0
 }*/

void realizarOperacionDeListener(int nuevoSocket){
	int identificador;

	aceptarConexion(nuevoSocket, cliente_dir);

	valorRtaRecv = recv(nuevoSocket,&identificador,sizeof(int), 0);

	esErrorConSalida(valorRtaRecv, "Error en recv ID");

	operacionSegunIdentificador(identificador);
}


void operacionSegunIdentificador(int identificador){
	switch (identificador) {

						case ID_CONSOLA:

							//Si la conexion es una consola, agregamos el socket a relavantes y enviamos mensaje de aceptacion.

							FD_SET(nuevoSocket, &socketsRelevantes);

							send(nuevoSocket,"Conexion aceptada. Bienvenido, proceso Consola",strlen("Conexion aceptada. Bienvenido, proceso Consola"),0);

							//Actualizando el maximo descriptor de fichero.

							if (nuevoSocket > fileDescMax)

								fileDescMax = nuevoSocket;

							agregarALista(ID_CONSOLA, nuevoSocket);

							//mensaje de nueva conexion de consola.

							printf("kernel: Nueva conexion de una consola, ip:%s en el socket %d\n",inet_ntoa(cliente_dir.sin_addr),nuevoSocket);

							break;

						case ID_CPU:

							//Si la conexion es una cpu, agregamos el socket a relavantes y enviamos mensaje de aceptacion.

							FD_SET(nuevoSocket, &socketsRelevantes);

							send(nuevoSocket,"Conexion aceptada. Bienvenido, proceso Cpu",strlen("Conexion aceptada. Bienvenido, proceso Cpu"),

							0);

							//Actualizando el maximo descriptor de fichero.

							if (nuevoSocket > fileDescMax)

								fileDescMax = nuevoSocket;

							agregarALista(ID_CPU, nuevoSocket);

							//mensaje de nueva conexion de cpu.

							printf("kernel: Nueva conexion de un cpu, ip:%s en el socket %d\n", inet_ntoa(cliente_dir.sin_addr),nuevoSocket);

							break;

						default:

							printf("Identificación incorrecta");

							break;

						} // cierra switch



}

