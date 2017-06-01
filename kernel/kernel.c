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

#define RUTA_ARCHIVO "./config_kernel.cfg"
#define SIZE_DATA 1024
#define IDCONSOLA 1
#define IDCPU 2

int ordenDeConsola;
int identificador;
int contadorPid = 1;

// Estructuras de datos

typedef struct{
	int 					pagina;
	t_puntero_instruccion 	offset;
	t_size 					size;
} posicionMemoria;

//Estructura para manejar una posición de memoria.

typedef struct{
	t_nombre_variable 	nombre;
	posicionMemoria 	posicionMemoria;
} variableStack ;

//Estructura de la lista de variables que hay en el stack

typedef struct{
	t_puntero_instruccion	primerInstruccion;	// El numero de la primera instrucción (Begin).
	t_size					instruccionesSize;	// Cantidad de instrucciones del programa.
	t_intructions*			instrucciones; 		// Instrucciones del programa.
} indiceCodigo;

// El índice de Código del PCB con las líneas útiles de código.

typedef struct{
	t_size			etiquetasSize;		// Tamaño del mapa serializado de etiquetas.
	char*			etiquetas;			// La serializacion de las etiquetas.
	int				cantidadFunciones;
	int				cantidadEtiquetas;
} indiceEtiqueta;

// El índice de Etiqueta del PCB para poder identificar las funciones de un programa.

typedef struct{
	t_list*			argumentos; // Posiciones de memoria donde se almacenan las copias de los argumentos de la función.
	t_list*			variables; 	// Identificadores y posiciones de memoria donde se almacenan las variables locales de la función.
	t_puntero		retPos; 	// Posición del índice de código donde se debe retornar al finalizar la ejecución de la función.
	posicionMemoria	retVar; 	// Posición de memoria donde se debe almacenar el resultado de la función provisto por la sentencia RETURN.
} indiceStack;

// El índice de Stack del PCB para poder hacer llamadas a procedimientos con sus argumentos.

typedef struct{
	int 				pid; 				// Identificador de un proceso.
	int 				programCounter; 	// Program counter: indica el número de la siguiente instrucción a ejecutarse.
	int 				paginasUsadas; 		// Cantidad de páginas usadas por el programa (Desde 0).
	t_list*		        indiceCodigo;		// Identifica líneas útiles de un programa
	t_list*		        indiceEtiqueta;		// Identifica llamadas a funciones.
	t_list*		        indiceStack; 		// Ordena valores almacenados en la pila de funciones con sus valores.
	int 				exitCode; 			// Motivo por el cual finalizó un programa.
} pcb;

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
	int validacionDeMemoria =0;
	char* handshake= "Kernel conectado";
	char buffer[1024]; //Buffer de datos
	int tamanioDePagina;
	memset (buffer,'\0',SIZE_DATA);

	struct sockaddr_in kernel_dir, cliente_dir, filesystem_dir,memoria_dir;
	struct timeval tv;



t_list *consolas = list_create();
t_list *cpus = list_create();
t_list *pcbs = list_create();



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
    //obtener el tamaño de pagina
	valorRtaRecv= recv (sockMemoria,tamanioDePagina,sizeof(int),0);
	esErrorConSalida(valorRtaRecv,"Error al obtener el tamaño de pagina");

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
//hilos
//hilo de interfaz de consola
generarMenu();


printf ("Consola de kernel"/n);


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
											
											case IDCPU{
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
		
void obtenerOrden(int* orden){
		printf("Elija una opcion\n");
		scanf("%d",orden);
		}

void incrementarcontadorPid (contadorPid){
	contadorPid++;
}
/*
bool compararSockets(int socket1, int socket2){
	return socket1==socket2 ? true : false
}*/
