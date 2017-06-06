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
#include "../librerias/pcb.h"

#include "./funcionesKernel.h"

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

	tv.tv_sec = 2;
	tv.tv_usec = 500000;
	
	typedef struct {
		int socketcpu;
		pcb pcbAsociado;
	}cpuAsociadoAPcb;

	typedef struct {
		int estado =0;
		int socket;
	}consola_activa;

	typedef struct {
		int estado =0;
		int socket;
	}cpu_activo;



// ************** MAIN *********************
int main(int argc, char *argv[])
{


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
	int sockListener;
	int sockFS;
	//*Socket para aceptar
	int nuevoSocket;
	int longitudBytesRecibidos;
	int yes = 1;
	int longitudEstructuraSocket;
	//*Contadores del for
	int i, j;
	int validacionDeMemoria =0;
	char* handshake= "Kernel conectado";



	struct timeval tv;


t_list *procesos = list_create();
t_list *consolas = list_create();
t_list *cpus = list_create();
t_list *pcbs = list_create();

  
	typedef struct {
		int pid;
    int rafagasEjecutadas=0;
    int syscallsEjecutadas=0;
    

	}informacionDeProceso;


	iniciarConfiguraciones();



//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

	int valorRtaSetSockOpt = 0;
	int valorRtaConnect=0;
	int valorRtaRecv=0;
	int valorRtaSend= 0;
	int valorRtaSelect=0;
	int valorRtaBind=0;
	int valorRtaListen = 0;


//CONEXION AL PROCESO MEMORIA
	int rtaHandshake;
	sockMemoria = asignarSocketMemoria();
  hanshake(sockMemoria);
  setFramesize(obtenerTamanioPagina(sockMemoria));
  
  FD_SET(sockMemoria, &socketsRelevantes);

//CONEXION AL PROCESO FILE SYSTEM

	sockFS = asignarSocketFS();
 	handshake(sockFS)


	FD_SET(sockFileSystem, &socketsRelevantes);
 
//CREACION DEL SOCKET ESCUCHA Y VERIFICACION DE ERROR

  
  sockListener = asignarSockeListener();

	//*Se agregan el listener a los sockets relevantes y se lo asigna como maximo por ser el unico en la lista en este momento
	FD_SET(sockListener, &socketsRelevantes);
	fileDescMax = sockListener;

printf ("Consola de kernel"/n);
	pthread_t idHilo;
  pthread_create(&idHilo, NULL, consolaKernel,NULL);
  

//hilo de escucha 
	while (1){
		memset(buffer,'\0',SIZE_DATA);
		socketsFiltrados = socketsRelevantes;

		valorRtaSelect = select(fileDescMax + 1, &socketsFiltrados, NULL, NULL, &tv);
		esErrorConSalida(valorRtaSelect,"Error en select");

		//int nroConsola=0;
		for (i = 0; i <= fileDescMax; i++){

			if(FD_ISSET(i, &socketsFiltrados)){

				if(i == sockListener){
					
					longitudEstructuraSocket = sizeof(cliente_dir);
					nuevoSocket = accept(sockListener,(struct sockaddr *) &cliente_dir,&longitudEstructuraSocket);
					esErrorConSalida(nuevoSocket,"error en accept");
					valorRtaRecv=recv(nuevoSocket,identificador,sizeof(int),0);
					esErrorConSalida(valorRtaRecv, "Error en recv ID");
					
										switch(identificador){
											case IDCONSOLA:
													//Si la conexion es una consola, agregamos el socket a relavantes y enviamos mensaje de aceptacion.
													FD_SET(nuevoSocket, &socketsRelevantes);
													send(nuevoSocket, "Conexion aceptada. Bienvenido, proceso Consola", strlen("Conexion aceptada. Bienvenido, proceso Consola"), 0);

													//Actualizando el maximo descriptor de fichero.
													if(nuevoSocket > fileDescMax)
													fileDescMax = nuevoSocket;
													
													agregarALista(IDCONSOLA,nuevoSocket,consolas);
													//mensaje de nueva conexion de consola.
													printf("%s: Nueva conexion de una consola, ip:%s en el socket %d\n",argv[0], inet_ntoa(cliente_dir.sin_addr), nuevoSocket);
													break;
											
                      case IDCPU:
												//Si la conexion es una cpu, agregamos el socket a relavantes y enviamos mensaje de aceptacion.
													FD_SET(nuevoSocket, &socketsRelevantes);
													send(nuevoSocket, "Conexion aceptada. Bienvenido, proceso Cpu", strlen("Conexion aceptada. Bienvenido, proceso Cpu"), 0);
													
												//Actualizando el maximo descriptor de fichero.
													if(nuevoSocket > fileDescMax)
													fileDescMax = nuevoSocket;
												agregarALista(IDCPU,nuevoSocket,cpus);
												//mensaje de nueva conexion de cpu.
													printf("%s: Nueva conexion de un cpu, ip:%s en el socket %d\n", argv[0], inet_ntoa(cliente_dir.sin_addr), nuevoSocket);
												break;
											default:
												printf("Identificación incorrecta");
												break;
							}

						}
				else
					{
						//Si no es listener, es consola, cpu, o filesystem


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
						pcb nuevoPCB;
						nuevoPCB.pid= contadorPid;
						incrementarcontadorPid();
						nuevoPCB.programCounter = 0;
						tamañoPath = calcularTamañoPath (Path);
						nuevoPCB.paginasUsadas= tamañoPath/tamañoPagina;
						//nuevoPCB.indiceCodigo = 
						//nuevoPCB.indiceEtiqueta=
						//nuevoPCB.indiceStack=
						//nuevoPCB.exitCode=
						
						//solicitar memoria
						send(sockMemoria, nuevoPCB.pid, sizeof(int), 0);
						send(sockMemoria, nuevoPCB.paginasUsadas, sizeof(int), 0);
						recv(sockMemoria,&validacionDeMemoria,sizeof(int),0);
						
						//enviar pcb mas codigo a memoria
						
						//elegir cpu_activa
						//enviar a cpu_activa

								//terminar programa

						}
					
					
					
								 	 	 	 	 	 	 	 }

														}//cierra - if(FD_ISSET(j, &socketsRelevantes))

							 					}//cierra - for(j = 0; j <= fileDescMax; j++)

						   	   }//cierra - else puts(buffer)

					 	 }//cierra - else longitud bytes recibidos

				   }//if(FD_ISSET(i, &socketsFiltrados))

			}//cierra -  for(i = 0; i <= fileDescMax; i++)

		}//cierra -  while(1)
    pthread_join(&idHilo, NULL);
	   return 0;
}

//Agregas un elemento a la lista segun el tipo
void agregarALista(int tipo, int socketDato,t_list *lista){
	
	//si tipo igual a cero, se crea una consola
	if(tipo==1){
		consola_activa aux;
		aux.socket=socketDato;
			list_add(lista,&aux);
	}
	if(tipo==2){
		cpu_activo aux;
		aux.socket=socketDato;
			list_add(lista,&aux);
	}

}

void consolaKernel(){

int ordenDeConsola;
//hilo de interfaz de consola
generarMenu();
obtenerOrden(&ordenDeConsola);
switch (ordenDeConsola){
	case 1:
	//mostrar listado de procesos del sistema\n
	case 2:
		generarMenuDeProceso();
		obtenerOrden(&ordenDeConsolaParaProceso);
			switch (ordenDeConsolaParaProceso){
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
					obtenerOrden(&ordenDeHeap);
					if (ordenDeHeap==1){
						//mostrar cantidad de acciones alojar ejecutadas por el proceso
					}
					else{
						//mostrar cantidad de acciones Liberar ejecutadas por el proceso
					}
					break;
				case 5:
					//mostrar cantidad de syscalls ejecutadas
					break;
				default:
					break;
				}
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

/*
bool compararSockets(int socket1, int socket2){
	return socket1==socket2 ? true : false
}*/
-