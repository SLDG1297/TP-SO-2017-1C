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


#define RUTA_ARCHIVO "./config_memoria.cfg"
#define SIZE_DATA 1024
#define RUTA_LOG "./memoriaLog.txt"

//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA



//*Socket para entablar conexion con el kernel
int sockAlKernel;
int yes = 1;
struct sockaddr_in espera, datosDelKernel;
int socketServidor;

//*Puntero a memoria contigua
int *ptrMemoria;

t_log* archivoLog;

//DECLARACION DE PROTOTIPO DE FUNCIONES

int crearSocket();
void laConexionFueExitosa(char* handshake);
void iniciarConexion();

//************** MAIN **************//
int main(int argc, char *argv[]) {

//DECLARACION DE VARIABLES PARA EL CODIGO PRINCIPAL

//*Socket para entablar conexion con el kernel
	char buffer[SIZE_DATA];
//	char* handshake = "La conexion al proceso memoria fue exitosa";
	memset(buffer, '\0', SIZE_DATA);



// CODIGO PRINCIPAL DE LA CONSOLA
	//*solicitud de bloque de memoria contigua
	asignarDatosDeConfiguracion();


	ptrMemoria =reservarMemoria();
	archivoLog=crearArchivo(RUTA_LOG);
	inicializarStrAdm(ptrMemoria);
	imprimirStrAdm(ptrMemoria);

	socketServidor=iniciarConexionServidor();



	//*asignacion de datos al socket que espera a la conexion del kernel

	//DIRECCION PARA ESPERAR A LA CONEXION CON EL KERNEL
	/*iniciarConexion();

	 laConexionFueExitosa(handshake);


	 puts("La conexion al proceso kernel fue exitosa\n");
	 puts("Esperando mensajes\n");
	 */

	return 0;

}

int iniciarConexionServidor() {

	int socketEsperaKernel = crearSocket();
	printf("Setsockopt correcto\n");
	puts("Socket de espera creado\n");

	//Declaracion de variables para recibir valor de rta
	int valorRtaBind = 0;
	int valorRtaListen = 0;

	int longitudEstructuraSocket = sizeof(datosDelKernel);
	//DIRECCION PARA ESPERAR A LA CONEXION CON EL KERNEL

	espera.sin_family = AF_INET;
	espera.sin_port = htons(getPuertoMemoria());
	espera.sin_addr.s_addr = INADDR_ANY;
	bzero(&(espera.sin_zero), 8);

//Bind(): Asigna la direccion de addr al socket referido por el SocketEsperaKernel
	valorRtaBind = bind(socketEsperaKernel, (struct sockaddr *) &espera,
			sizeof(struct sockaddr));
	errorSalidaSocket(valorRtaBind, "Error en Bind", &socketEsperaKernel);

	printf("Bind correcto\n");
	puts("Esperando la conexion del kernel\n");

//Listen(): Marca al socket como pasivo, es decir un socket que sera utilizado para aceptar conecciones
	valorRtaListen = listen(socketEsperaKernel, 5);
	errorSalidaSocket(valorRtaListen, "Error en Listen", &socketEsperaKernel);

//Accept(): Extrae el primer pedido de conexion de la cola de conexiones pendientes, del socket de escucha
//crea un nuevo socket conecatado y retorna un nuevo descriptor de archivo referido a ese socket.
	sockAlKernel = accept(socketEsperaKernel,
			(struct sockaddr *) &datosDelKernel, &longitudEstructuraSocket);
	errorSalidaSocket(sockAlKernel, "Error en el accept", &socketEsperaKernel);

	return sockAlKernel;
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
void laConexionFueExitosa(char* handshake) {

	int longitudDatosEnviados;
	longitudDatosEnviados = send(sockAlKernel, handshake, strlen(handshake), 0);
	esErrorSinSalida(longitudDatosEnviados, "Fallo en el handshake");

}


// ************************* OPERACIONES DE MEMORIA *************************************

/*
 * Cuando el KRN comunique el inicio de un nuevo prog se crearan las estructuras necesarias para administrarlo correctamente. En una misma pag
 * no se podran tener datos referentes a dos segmentos diferentes
 */

int inicializarPrograma(int pid, int cantPaginas){

	iniciarStrAdmProg(pid, cantPaginas,ptrMemoria);

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




