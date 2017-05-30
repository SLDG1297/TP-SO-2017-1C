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
#include <semaphore.h>

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

//***INCLUDE LIBRERIAS PROPAIS
#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/controlErrores.h"
#include "../librerias/funcionesMemoria.h"


//***DEFINE DATOS ESTATICOS
#define RUTA_ARCHIVO "./config_memoria.cfg"
#define SIZE_DATA 1024
#define RUTA_LOG "./memoriaLog.txt"

//***DATOS PARA ENTABLAR CONEXION CON EL KERNEL

int yes = 1;
struct sockaddr_in espera, datosDelKernel;
int socketServidor;

//***PUNTERO A BLOQUES DE MEMORIA
int *ptrMemoria;
int *ptrCache;

//***ARCHIVO LOG
t_log* archivoLog;

//***SEMAFORO
sem_t semaforo;

//***DECLARACION DE PROTOTIPO DE FUNCIONES

int crearSocket();
void laConexionFueExitosa(char* handshake,int *socket);
int iniciarConexionServidor();
int ponerSocketEnEscucha_aceptarConexion();
void *asignarSocketAConexion();
void *seleccionOperacionesMemoria(int *socket);

//************** MAIN **************//
int main(int argc, char *argv[]) {

//DECLARACION DE VARIABLES PARA EL CODIGO PRINCIPAL

// declaramos el hilo y sus atributos
	pthread_t idHilo;
	char buffer[SIZE_DATA];
	char* handshake = "\nLa conexion al proceso memoria fue exitosa";
	memset(buffer, '\0', SIZE_DATA);



// CODIGO PRINCIPAL DE LA CONSOLA
	//*solicitud de bloque de memoria contigua
	asignarDatosDeConfiguracion();


	ptrMemoria =reservarMemoria();
	archivoLog=crearArchivo(RUTA_LOG);
	inicializarStrAdm(ptrMemoria);
	imprimirStrAdm(ptrMemoria);

	//* creamos un socket con el cual vamos a manejar la conexion con el kernel
	socketServidor=iniciarConexionServidor();
	//laConexionFueExitosa(handshake,&socketServidor);
	puts("La conexion al proceso kernel fue exitosa\n");
	puts("Esperando mensajes\n");

	//*Creo un hilo que se encargue unicamente de manejar las conexiones a la memoria
	// Seteamos los atributos (2do parametro) como nulo, ya que de esta manera los atributos son por defecto

	// int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);
	if(pthread_create(&idHilo,NULL,asignarSocketAConexion,NULL)<0){
		printf("No se pudo crear el hilo para asignar un socket a la conexion");
	}


	 //int pthread_join(pthread_t thread, void **retval);
	 //* Finalmente esperamos a que el hilo original termine

	 pthread_join(idHilo,NULL);

	return 0;

}

int iniciarConexionServidor() {

	int socketEsperaKernel = crearSocket();
	printf("Setsockopt correcto\n");
	puts("Socket de espera creado\n");

	//Declaracion de variables para recibir valor de rta
	int valorRtaBind = 0;
//	int valorRtaListen = 0;

//	int longitudEstructuraSocket = sizeof(datosDelKernel);
	//DIRECCION PARA ESPERAR A LA CONEXION CON EL KERNEL

	espera.sin_family = AF_INET;
	espera.sin_port = htons(getPuertoMemoria());
	espera.sin_addr.s_addr = INADDR_ANY;
	bzero(&(espera.sin_zero), 8);

//Bind(): Asigna la direccion de addr al socket referido por el SocketEsperaKernel
	valorRtaBind = bind(socketEsperaKernel, (struct sockaddr *) &espera,
			sizeof(struct sockaddr));
	errorSalidaSocket(valorRtaBind, "Error en Bind", (int *) socketEsperaKernel);

	printf("Bind correcto\n");
	printf("Socket asignado: %d\n",socketEsperaKernel);
	puts("Esperando la conexion del kernel\n");

////Listen(): Marca al socket como pasivo, es decir un socket que sera utilizado para aceptar conecciones
//	valorRtaListen = listen(socketEsperaKernel, 5);
//	errorSalidaSocket(valorRtaListen, "Error en Listen", &socketEsperaKernel);
//
////Accept(): Extrae el primer pedido de conexion de la cola de conexiones pendientes, del socket de escucha
////crea un nuevo socket conecatado y retorna un nuevo descriptor de archivo referido a ese socket.
//	sockAlKernel = accept(socketEsperaKernel,
//			(struct sockaddr *) &datosDelKernel, &longitudEstructuraSocket);
//	errorSalidaSocket(sockAlKernel, "Error en el accept", &socketEsperaKernel);

	return socketEsperaKernel;
}
int crearSocket() {
	//Declaracion de variable para recibir valor de rta
	int valorRtaSetSockOpt;
	int sockDeEspera;

	//Socket(): creates an endpoint for communication and returns a file descriptor that refers to that endpoint.

	sockDeEspera = socket(AF_INET, SOCK_STREAM, 0);
	errorSalidaSocket(sockDeEspera, "Fallo en la creacion del socket de espera",
			&sockDeEspera);

	valorRtaSetSockOpt = setsockopt(sockDeEspera, SOL_SOCKET, SO_REUSEADDR,
			(const char*) &yes, sizeof(yes));
	errorSalidaSocket(valorRtaSetSockOpt,
			"Error en setSockOpt en el socket de espera", &sockDeEspera);
	return sockDeEspera;
}

void laConexionFueExitosa(char* handshake,int *socket) {

	int longitudDatosEnviados = send(*socket, handshake, sizeof(char)*strlen(handshake), 0);
	esErrorSinSalida(longitudDatosEnviados, "Fallo en el handshake");

}

//Esta funcion se encarga de asignar un socket a un conexion aceptada y crear un hilo que se encargue especificamente de esa conexion
//para recibir las operacion de memoria que el emisor desea realizar

void *asignarSocketAConexion(){

	pthread_t idHilo_OPERACIONES;
	int valorRtaThread;
	int socketRecepcion;
	socketRecepcion=ponerSocketEnEscucha_aceptarConexion();

	while(1){
		sem_wait(&semaforo);
		//Creamos un segundo hilo que se encargue de los pedidos operaciones especificas de la memoria segun pedido de cada conexion
		valorRtaThread=pthread_create(&idHilo_OPERACIONES,NULL,seleccionOperacionesMemoria, &socketRecepcion);
		esErrorSimple(valorRtaThread,"No se pudo crear el hilo, dios sabra por que?");
	}

	pthread_join(idHilo_OPERACIONES,NULL);

}

void *seleccionOperacionesMemoria(int *socket){

//	int seleccion;
//	seleccion=recibirSeleccionOperacion(socket);


	return 0;
}

//Este metodo pone al socket en escuchar, con una lista de espera de 5 y acepta la primera conexion de la misma lista, como ultima instancia devuelve
//el socket correspondiente a la conexion aceptada
int ponerSocketEnEscucha_aceptarConexion(){
	int valorRtaListen,conexionAceptada;
	struct sockaddr strAddr;
	socklen_t sizeStrAddr = sizeof(strAddr);

	valorRtaListen=listen(socketServidor,5);
	esErrorSinSalida(valorRtaListen,"No se ha podido poner el socket en escucha");

	conexionAceptada=accept(socketServidor,(struct sockaddr*) &strAddr, &sizeStrAddr);
	esErrorSinSalida(conexionAceptada,"No se ha podido aceptar la conexion en espera");

	printf("Socket asignado a conexion aceptada: %d", conexionAceptada);
	return conexionAceptada;
}

// ************************* OPERACIONES DE MEMORIA *************************************




/*
 * Cuando el KRN comunique el inicio de un nuevo prog se crearan las estructuras necesarias para administrarlo correctamente. En una misma pag
 * no se podran tener datos referentes a dos segmentos diferentes
 */

int inicializarPrograma(int pid, int cantPaginas){

	iniciarStrAdmDeProg(pid, cantPaginas,ptrMemoria);

	return 0;
}


/*
 * Ante un pedido de lectura de paginas de alguno de los PRCs de la CPU, se realizara la traduccion a MARCO/FRAME y se devolvera el contenido
 * correspondiente consultado primeramente a la M. Cache, en caso de que esta no contenga la info se informara un Cache Miss, se debera cargar
 * la pagina en cache y se devolvera la informacion solicitada
 */
//char* solicitarBytesDePagina(int pid, int nroPagina, int offset, int size ){}

/*
 * Ante un pedido de escritura de paginas de alguno de los PRCDs, se realizara la traduccion a MARCO/FRAME y se actualizara su contenido
 * En caso de que la pag se encuentr en la M. Cache se debera actualizar tambien el frame alojado en la misma
 */
//int almacenarBytesEnPagina(int pid, int nroPagina, int offset, int size, char* buffer){}


/*
 *El PRC memoria debera aignarle tantas paginas como se soliciten, al PRC ampliando asi su tamaño. En caso de que no se
 *pueda asignar ese espacio, se debera informar al KRN la falta de espacio
 */
//int asignarPaginasAProceso(int pid, int paginasNecesarias){}


//Cuando el PRC KRN informe el fin de un prog. se deben eliminar las entradas en estructuras usadas de memoria
int finalizarPrograma (int pid){

 liberarStrAdm(pid,ptrMemoria);
 printf("\nEl programa %d ha finalizado",pid);
 return 0;
}




