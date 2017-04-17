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

#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/controlErrores.h"

#define RUTA_ARCHIVO "./config_kernel.cfg"
#define SIZE_DATA 1024



int main(int argc, char *argv[])
{
	//CODIGO PARA LLAMAR AL ARCHIVO

		//Estructura para manejar el archivo de configuración -- t_config*
		//Crear estructura de configuración para obtener los datos del archivo de configuración.

		t_config* configuracion;
		char* ruta = RUTA_ARCHIVO;
		configuracion = llamarArchivo(ruta);

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
	int sockMemoria;
	int sockFileSystem;
	//*Socket para aceptar
	int nuevoSocket;
	int longitudBytesRecibidos;
	int yes = 1;
	int longitudEstructuraSocket;
	//*Contadores del for
	int i, j;

	char* handshake= "Kernel conectado";
	char buffer[1024]; //Buffer de datos

	memset (buffer,'\0',SIZE_DATA);

	struct sockaddr_in kernel_dir, cliente_dir, filesystem_dir,memoria_dir;
	struct timeval tv;

	tv.tv_sec = 2;
	tv.tv_usec = 500000;

//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION

	int	PUERTO_KERNEL = busquedaClaveNumerica(configuracion,"PUERTO_KERNEL");
//	int PUERTO_CPU = busquedaClaveNumerica(configuracion,"PUERTO_CPU");
	int PUERTO_MEMORIA = busquedaClaveNumerica(configuracion,"PUERTO_MEMORIA");
	int PUERTO_FS = busquedaClaveNumerica(configuracion,"PUERTO_FS");
	char* IP_MEMORIA = busquedaClaveAlfanumerica(configuracion,"IP_MEMORIA");
	char* IP_FS = busquedaClaveAlfanumerica(configuracion,"IP_FS");

//	int QUANTUM = busquedaClaveNumerica(configuracion,"QUANTUM");
//	int QUANTUM_SLEEP = busquedaClaveNumerica(configuracion,"QUANTUM_SLEEP");
//	int GRADO_MULTIPROG= busquedaClaveNumerica(configuracion,"GRADO_MULTIPROG");
//	int SEM_INIT [3];
//	int STACK_SIZE= busquedaClaveNumerica(configuracion,"STACK_SIZE");

//	char* ALGORITMO = busquedaClaveAlfanumerica(configuracion, "ALGORITMO");
//	char* SEM_IDS [3];
//	char* SHARED_VARS[3];


//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

	int valorRtaSetSockOpt = 0;
	int valorRtaConnect=0;
	int valorRtaRecv=0;
	int valorRtaSend= 0;
	int valorRtaSelect=0;
	int valorRtaBind=0;
	int valorRtaListen = 0;


//CODIGO PRINCIPAL DE LA CONSOLA


//CONEXION AL PROCESO MEMORIA

	sockMemoria = socket (AF_INET,SOCK_STREAM,0);
	//esErrorConSalida controla si el resultado obtenido por una funcion X fue un error, siendo el error considerado con el valor -1
	esErrorConSalida(sockMemoria,"Fallo en la creacion del socket a memoria");

	valorRtaSetSockOpt = setsockopt(sockMemoria, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	esErrorConSalida(valorRtaSetSockOpt,"Error en el setsockopt del socket memoria");

//ASIGNACION DE DATOS DE LA MEMORIA
    memoria_dir.sin_family = AF_INET;
    memoria_dir.sin_port = htons(PUERTO_MEMORIA);
    memoria_dir.sin_addr.s_addr = inet_addr(IP_MEMORIA);
    bzero(&(memoria_dir.sin_zero), 8);

	puts("Enviando conexion a proceso memoria\n");

	valorRtaConnect=connect(sockMemoria, (struct sockaddr *)&memoria_dir, sizeof(struct sockaddr));
	esErrorConSalida(valorRtaConnect,"Error al conectarse al proceso memoria");

    valorRtaRecv = recv(sockMemoria,buffer,sizeof(buffer),0);
	esErrorConSalida(valorRtaRecv,"Error en el handshake de memoria (recepcion)");

	valorRtaSend = send(sockMemoria,handshake,strlen(handshake),0);
	esErrorConSalida(valorRtaSend,"Error en el handshake de memoria (envio)");


    puts(buffer);
    FD_SET(sockMemoria, &socketsRelevantes);
    memset (buffer,'\0',SIZE_DATA);


//CONEXION AL PROCESO FILE SYSTEM

	sockFileSystem = socket (AF_INET,SOCK_STREAM,0);
	esErrorConSalida(sockFileSystem,"Fallo en la creacion del socket fileSystem");


	valorRtaSetSockOpt = setsockopt(sockFileSystem, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	esErrorConSalida(valorRtaSetSockOpt,"Error en el setsockopt del socket fileSystem");

//ASIGNACION DE DATOS PARA EL FILE SYSTEM
    filesystem_dir.sin_family = AF_INET;
    filesystem_dir.sin_port = htons(PUERTO_FS);
    filesystem_dir.sin_addr.s_addr = inet_addr(IP_FS);
    bzero(&(filesystem_dir.sin_zero), 8);

    puts("Enviando conexion a proceso FileSystem\n");

    valorRtaConnect =connect(sockFileSystem, (struct sockaddr *)&filesystem_dir, sizeof(struct sockaddr));
	esErrorConSalida(valorRtaConnect,"Error al conectarse al proceso fileSystem");

    valorRtaRecv=recv (sockFileSystem,buffer,sizeof(buffer),0);
	esErrorConSalida(valorRtaRecv,"Error en el handshake de fileSystem (recepcion)");


	valorRtaSend = send(sockFileSystem,handshake,strlen(handshake),0);

	esErrorConSalida(valorRtaSend,"Error en el handshake de fileSystem (envio)");

	puts(buffer);
	FD_SET(sockFileSystem, &socketsRelevantes);
	    memset (buffer,'\0',SIZE_DATA);



//CREACION DEL SOCKET ESCUCHA Y VERIFICACION DE ERROR

	sockListener = socket(AF_INET, SOCK_STREAM, 0);
	esErrorConSalida(sockListener,"Error en el Socket ");


	valorRtaSetSockOpt=setsockopt(sockListener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ;
	esErrorConSalida(valorRtaSetSockOpt,"Error en setsockopt");

	printf("Setsockopt correcto\n");
	puts("Socket listener creado");

//ASIGNACION DE DATOS PARA EL KERNEL

	kernel_dir.sin_family = AF_INET;
	kernel_dir.sin_port = htons(PUERTO_KERNEL); //Aca se pone el puerto del listener
	kernel_dir.sin_addr.s_addr = INADDR_ANY;
	bzero(&(kernel_dir.sin_zero), 8);

	//*Se asocia el listener al puerto de escucha

	valorRtaBind =bind(sockListener, (struct sockaddr *) &kernel_dir,sizeof(struct sockaddr));
	esErrorConSalida(valorRtaBind,"Error en Bind");


	printf("Bind correcto\n");

	valorRtaListen =listen(sockListener, 10);
	esErrorConSalida(valorRtaListen,"Error en listen");


	printf("Listen correcto\n");
	printf("\nEsperando en el puerto %i\n", PUERTO_KERNEL);

	//*Se agregan el listener a los sockets relevantes y se lo asigna como maximo por ser el unico en la lista en este momento
	FD_SET(sockListener, &socketsRelevantes);
	fileDescMax = sockListener;

	while (1){
		memset(buffer,'\0',SIZE_DATA);
		socketsFiltrados = socketsRelevantes;

		valorRtaSelect = select(fileDescMax + 1, &socketsFiltrados, NULL, NULL, &tv);
		esErrorConSalida(valorRtaSelect,"Error en select");

		for (i = 0; i <= fileDescMax; i++){

			if(FD_ISSET(i, &socketsFiltrados)){

				if(i == sockListener){

					longitudEstructuraSocket = sizeof(cliente_dir);
					nuevoSocket = accept(sockListener,(struct sockaddr *) &cliente_dir,&longitudEstructuraSocket);

					if (nuevoSocket == -1)
						perror("Error en accept");

					else{

							printf("Accept correcto\n");
							FD_SET(nuevoSocket, &socketsRelevantes);
							send(nuevoSocket, "Conexion aceptada", sizeof("Conexion aceptada"), 0);

							//Actualizando el maximo descriptor de fichero

							if(nuevoSocket > fileDescMax)
								  fileDescMax = nuevoSocket;

							printf("%s: Nueva conexion de %s en el socket %d\n", argv[0], inet_ntoa(cliente_dir.sin_addr), nuevoSocket);

							}

						}
				else
					{
					longitudBytesRecibidos = recv(i, buffer, sizeof(buffer), 0);

					if(longitudBytesRecibidos<= 0){

						//Error o fin de conexion?

						if(longitudBytesRecibidos == 0)
							printf("%s: Socket %d colgado\n", argv[0], i); //Conexion cerrada

						else
							perror("Error en el recv");

						//Se cierra el socket
						close(i);
						FD_CLR(i, &socketsRelevantes);
						}
						else{

						puts(buffer);
						  for(j = 0; j <= fileDescMax; j++){
							if(FD_ISSET(j, &socketsRelevantes)){
							   //Reenviar el mensaje a todos menos al listener y al socket que recibio el mensaje
								 if(j != sockListener && j != i ) {
								   valorRtaSend = send(j, buffer, longitudBytesRecibidos, 0);
									 esErrorSinSalida(valorRtaSend,"Error en Send");
								 	 	 	 	 	 	 	 }

														}//cierra - if(FD_ISSET(j, &socketsRelevantes))

							 					}//cierra - for(j = 0; j <= fileDescMax; j++)

						   	   }//cierra - else puts(buffer)

					 	 }//cierra - else longitud bytes recibidos

				   }//if(FD_ISSET(i, &socketsFiltrados))

			}//cierra -  for(i = 0; i <= fileDescMax; i++)

		}//cierra -  while(1)
	   return 0;
}