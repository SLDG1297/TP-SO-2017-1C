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
int asignadorDePid = 1;


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

	//DECLARCION DE COLAS PARA PROGRAMACION
	t_queue *colaNew;
	t_queue *colaReady;
	t_queue *colaExecute;
	t_queue *colaBlock;
	t_queue *colaExit;



	typedef struct {
		int pid;
		int rafagasEjecutadas;
		int syscallsEjecutadas;

	} informacionDeProceso;

//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

		int valorRtaRecv = 0;
		int valorRtaSelect = 0;
//DELCARACION DE HILOS

		pthread_t idHilo, idHiloSelect, idHiloPlanificadorLargoPlazo;

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

	pthread_create(&idHilo, NULL, consolaOperaciones, NULL);
	pthread_create(&idHiloSelect, NULL, seleccionarSocketRelevantes,NULL);

	pthread_create(&idHiloPlanificadorLargoPlazo, NULL, planificadorLargoPlazo , NULL);


	//PLANIFICACION

	//DECLARACION Y CREACION DE COLAS

	colaNew = queue_create();
	colaReady = queue_create();
	colaExecute = queue_create();
	colaBlock = queue_create();
	colaExit = queue_create();





	pthread_join(idHiloSelect, NULL);
	pthread_join(idHilo, NULL);

	return 0;

}


int consultarDisponibilidadParaProcesos(/*tamaño necesario para el proceso */*programa){

	if(){
		return 1;

	}else{
		return 0;
	}

}



void *planificiadorLargoPlazo(){

	//grado de multiprogramacion
	extern int _gradoMultiprog;
	//el grado de multiprogramacion es el valor maximo del semaforo


	while(1){

	//se le pregunt a la memoria si hay lugar
	//semaforear


			//verificar que la cola no esta vacia todo

	int *programa = queue_peek(colaNew); //es el pid de un pcb	//se lo saca de colaNew

	int hayEspacio = consultarDisponibilidadParaProcesos(/*tamaño necesario para el proceso */*programa);


	//send


	//aca va a quedar esperando a que la memoria le indique que hay lugar
	//recv

	if(hayEspacio ==1){

		int *programa = queue_pop(colaNew); //es el pid de un pcb	//se lo saca de colaNew

		queue_push(colaReady, programa);//pone el proceso en colaReady

		//aumentar señal de que hay algo en ready

		}

	}

return 0;
}


//todo crear hilo
void *planificadorACortoPlazo(){

	int *programa;
	int algoritmoDePlanificion = 0; //leer archivo configuracion todo


	while(1){

		//esperar a señal de que hay algo en la cola de ready

		switch(algoritmoDePlanificacion){

		case '1':  //FIFO

			programa = queue_pop(colaReady);

			break;



		case '2':  //ROUND ROBIN

			break;

		default:

		}//fin del switch

	}//fin de while(1)

}


void* seleccionarSocketRelevantes(){
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

			}	//cierra - if(FD_ISSET(j, &socketsRelevantes))

		}	// cierra for for

	}	// cierra while
}

void realizarOperacionDeListener(int nuevoSocket){
	int identificador;

	aceptarConexion(nuevoSocket, cliente_dir);

	handshake(nuevoSocket);
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

