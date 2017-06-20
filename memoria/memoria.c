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
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/temporal.h>
#include <commons/collections/list.h>


//***INCLUDE LIBRERIAS PROPAIS
#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/controlErrores.h"
#include "../librerias/memoria/funcionesMemoria.h"
#include "../librerias/conexionSocket.h"
#include "../librerias/serializador.h"

//***DEFINE DATOS ESTATICOS
#define RUTA_ARCHIVO "./config_memoria.cfg"
#define SIZE_DATA 1024
#define RUTA_LOG "./memoriaLog.txt"

#define INICIAR_PROGRAMA 	51
#define SOLICITAR_BYTES_PAG 52
#define ALMACENAR_BYTES_PAG 53
#define ASIGNAR_PAGINAS_PRC 54
#define FINALIZAR_PRG 		55

//***DATOS PARA ENTABLAR CONEXION CON EL KERNEL

int yes = 1;
struct sockaddr_in direccionEspera, datosDelKernel;
int socketServidor;

//***PUNTERO A BLOQUES DE MEMORIA
int *ptrMemoria;
int *ptrCache;

char* buffer;

//***SEMAFORO
sem_t semaforo;

//***DECLARACION DE PROTOTIPO DE FUNCIONES

int crearSocket();
char* laConexionFueExitosa(int *socket);
int iniciarConexionServidor();
int ponerSocketEnEscucha_aceptarConexion();
void *asignarSocketAConexion();
void seleccionOperacionesMemoria(int socket);

int inicializarPrograma(int pid, int paginas);
char *solicitarBytesDePagina(int pid, int nroPagina, int offset, int size);
int almacenarBytesEnPagina(int pid, int nroPagina, int offset, int size,
		char* buffer);
int asignarPaginasProceso(int pid, int paginas);
int finalizarPrograma(int pid);

//************** MAIN **************//
int main(int argc, char *argv[]) {

//DECLARACION DE VARIABLES PARA EL CODIGO PRINCIPAL

// declaramos el hilo y sus atributos
	pthread_t idHilo;

// CODIGO PRINCIPAL DE LA CONSOLA
	//*solicitud de bloque de memoria contigua
	asignarDatosDeConfiguracion();

	ptrMemoria = reservarMemoria();
	crearArchivo(RUTA_LOG);
	inicializarStrAdm(ptrMemoria);
	imprimirStrAdm(ptrMemoria);

	//* creamos un socket con el cual vamos a manejar la conexion con el kernel
	socketServidor = iniciarConexionServidor();
	laConexionFueExitosa(&socketServidor);

	puts("La conexion al proceso kernel fue exitosa\n");
	puts("Esperando mensajes\n");

	//*Creo un hilo que se encargue unicamente de manejar las conexiones a la memoria
	// Seteamos los atributos (2do parametro) como nulo, ya que de esta manera los atributos son por defecto

	// int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg)
	if (pthread_create(&idHilo, NULL, asignarSocketAConexion, NULL) < 0)
		printf("No se pudo crear el hilo para asignar un socket a la conexion");
	else
		printf("hilo creado");

	//int pthread_join(pthread_t thread, void **retval);
	//* Finalmente esperamos a que el hilo original termine

	pthread_join(idHilo, NULL);
	free(ptrMemoria);
	free(ptrCache);
	return 0;

}

int iniciarConexionServidor() {

	int socketEsperaKernel = crearSocket();
	printf("Setsockopt correcto\n");
	puts("Socket de espera creado\n");

	//Declaracion de variables para recibir valor de rta
	int valorRtaBind = 0;

	//DIRECCION PARA ESPERAR A LA CONEXION CON EL KERNEL

    direccionEspera = crearDireccionServidor(getPuertoMemoria());

	/*
    direccionEspera.sin_family = AF_INET;
	direccionEspera.sin_port = htons(getPuertoMemoria());
	direccionEspera.sin_addr.s_addr = INADDR_ANY;
	bzero(&(direccionEspera.sin_zero), 8);
	*/
//Bind(): Asigna la direccion de addr al socket referido por el SocketEsperaKernel

    asociarDireccion(socketEsperaKernel,direccionEspera);

/*
    valorRtaBind = bind(socketEsperaKernel, (struct sockaddr *) &direccionEspera,
			sizeof(struct sockaddr));
	errorSalidaSocket(valorRtaBind, "Error en Bind",
			(int *) socketEsperaKernel);
*/

	printf("Socket asignado: %d\n", socketEsperaKernel);
	puts("Esperando la conexion del kernel\n");

	return socketEsperaKernel;
}


char* laConexionFueExitosa(int *socket) {

	int datosEnviados, datosRecibidos;
	size_t tamanio = sizeof(int);
	datosEnviados = send(*socket, getFrameSize(), tamanio, 0);
	esErrorConSalida(datosEnviados, "Error en el envio de datos: FRAME_SIZE");

	//modificar para el kernel
	void* datos = malloc(SIZE_DATA);
	tamanio = sizeof(datos);
	datosRecibidos = recv(*socket, datos, tamanio, 0);
	esErrorConSalida(datosRecibidos,
			"Error en la recepcion de datos: HANDSHAKE");
	char* buffer = *(char*) datos;
	free(datos);

	return buffer;
}

//Esta funcion se encarga de asignar un socket a un conexion aceptada y crear un hilo que se encargue especificamente de esa conexion
//para recibir las operacion de memoria que el emisor desea realizar

void *asignarSocketAConexion() {

	pthread_t idHilo_OPERACIONES;
	int valorRtaThread;
	int socketRecepcion;
	socketRecepcion = ponerSocketEnEscucha_aceptarConexion();

	while (1) {
		sem_wait(&semaforo);
		//Creamos un segundo hilo que se encargue de los pedidos operaciones especificas de la memoria segun pedido de cada conexion
		valorRtaThread = pthread_create(&idHilo_OPERACIONES, NULL,
				seleccionOperacionesMemoria, &socketRecepcion);
		//esErrorSimple(valorRtaThread,"No se pudo crear el hilo, dios sabra por que?");
		if (valorRtaThread == -1)
			printf("No se pudo crear el hilo, dios sabra por que?");
		else
			printf("Hilo correspondiente a socket %d creado", socketRecepcion);
	}

	pthread_join(idHilo_OPERACIONES, NULL);

}

//Este metodo pone al socket en escuchar, con una lista de direccionEspera de 5 y acepta la primera conexion de la misma lista, como ultima instancia devuelve
//el socket correspondiente a la conexion aceptada
int ponerSocketEnEscucha_aceptarConexion() {
	int valorRtaListen, conexionAceptada;
	struct sockaddr strAddr;
	socklen_t sizeStrAddr = sizeof(strAddr);

	//Listen(): Marca al socket como pasivo, es decir un socket que sera utilizado para aceptar conecciones
	valorRtaListen = listen(socketServidor, 5);
	esErrorSinSalida(valorRtaListen,
			"No se ha podido poner el socket en escucha");

	//Accept(): Extrae el primer pedido de conexion de la cola de conexiones pendientes, del socket de escucha
	//crea un nuevo socket conecatado y retorna un nuevo descriptor de archivo referido a ese socket.
	conexionAceptada = accept(socketServidor, (struct sockaddr*) &strAddr,
			&sizeStrAddr);
	esErrorSinSalida(conexionAceptada,
			"No se ha podido aceptar la conexion en direccionEspera");

	printf("Socket asignado a conexion aceptada: %d", conexionAceptada);
	return conexionAceptada;
}



// ************************* OPERACIONES DE MEMORIA *************************************


/*
 * Cuando el KRN comunique el inicio de un nuevo prog se crearan las estructuras necesarias para administrarlo correctamente. En una misma pag
 * no se podran tener datos referentes a dos segmentos diferentes
 */

int inicializarPrograma(int pid, int cantPaginas) {
	int valorRta;
	//Envio los datos hacia la funcion asignar paginas a prc ya que inicializar
	//un programa es asignarle las paginas en la str adm, asi que es lo mismo XD

	valorRta=asignarPaginasProceso(pid, cantPaginas);
	return valorRta;
}


/*
 * Ante un pedido de lectura de paginas de alguno de los PRCs de la CPU, se realizara la traduccion a MARCO/FRAME y se devolvera el contenido
 * correspondiente consultado primeramente a la M. Cache, en caso de que esta no contenga la info se informara un Cache Miss, se debera cargar
 * la pagina en cache y se devolvera la informacion solicitada
 */

char* solicitarBytesDePagina(int pid, int nroPagina, int offset, int size) {

	//1ro. Busco el frame correspondiente a la pagina que me enviaron
	int frame;
	frame = buscarFrameDePagina(pid, ptrMemoria, nroPagina);

	//2do. Si el frame es distinro a -28(NOT FOUND) copio los datos de la direccion
	//a donde apunta el ptr., hacia el  buffer

	if (frame != -28) {
		memcpy(&buffer, ptrMemoria + frame * getFrameSize() + offset, size);
		log_info(getArchivoLog(),
				"\nSe ha almacenado el valor: %s\nPID: %d \nFRAME: %d \nPAGINA: %d \nOFFSET: %d ",
				buffer, pid, frame, nroPagina, offset);
	} else
		log_warning(getArchivoLog(),
				"\nNo se encontro el frame correspondiente a la pagina %d del PID %d",
				nroPagina, pid);

	return buffer;
}


/*
 * Ante un pedido de escritura de paginas de alguno de los PRCDs, se realizara la traduccion a MARCO/FRAME y se actualizara su contenido
 * En caso de que la pag se encuentr en la M. Cache se debera actualizar tambien el frame alojado en la misma
 */
int almacenarBytesEnPagina(int pid, int nroPagina, int offset, int size,
		char* buffer) {

	//1ro. Busco el frame correspondiente a la pagina que me enviaron

	int frame = buscarFrameDePagina(pid, ptrMemoria, nroPagina);

	//2do. Si el frame es distinro a -28(NOT FOUND) copio los datos del buffer, hacia
	//la direccion a donde apunta el ptr.

	if (frame != -28) {
		memcpy(ptrMemoria + frame * getFrameSize() + offset, &buffer, size);
		log_info(getArchivoLog(),
				"\nSe ha almacenado el valor: %s\nPID: %d \nFRAME: %d \nPAGINA: %d \nOFFSET: %d ",
				buffer, pid, frame, nroPagina, offset);
		return 1;
	} else
		log_warning(getArchivoLog(),
				"\nNo se encontro el frame correspondiente a la pagina %d del PID %d",
				nroPagina, pid);
	return 0;
}


/*
 *El PRC memoria debera aignarle tantas paginas como se soliciten, al PRC ampliando asi su tamaño. En caso de que no se
 *pueda asignar ese espacio, se debera informar al KRN la falta de espacio
 */

int asignarPaginasProceso(int pid, int cantPaginas) {
	int asignacion;

	//1ro. Me fijo cuantas paginas libres hay en memoria

	int pagLibreMemoria = paginasLibresEnMemoria(ptrMemoria);

	//2do. Controlo si la cantidad de paginas libres es suficiente para las que necesita el prc

	if (pagLibreMemoria >= cantPaginas) {

	//3ro. asigno las paginas a las estructuras administrativas

		asignacion = asignacionDePaginas(cantPaginas, pid, ptrMemoria);

	//4to. Dependiento del rtado de ejecucion devuelve un 1 y un 0

		if (asignacion >= cantPaginas) {
			log_info(getArchivoLog(),
					"Se han asignado %d paginas, al PID %d en las estructuras administrativas",
					pid, asignacion);
			return 1;
		}

	}//Cierro if que evaluas las paginas libres contra las paginas libres de memoria
	else
		log_info(getArchivoLog(),
				"No hay paginas disponibles, PAGINAS LIBRES ACTUALES: %d",
				pagLibreMemoria);
	return 0;
}



//Cuando el PRC KRN informe el fin de un prog. se deben eliminar las entradas en estructuras usadas de memoria

int finalizarPrograma(int pid) {

	liberarStrAdm(pid, ptrMemoria);
	printf("\nEl programa %d ha finalizado", pid);
	return 1;
}

//Recibe la orden de operacion por medio de la seleccion y realiza la accion correspondiente segun el caso
void seleccionOperacionesMemoria(int socket) {

	int seleccion, pid, paginas, rtaFuncion, offset, size, nroFrame;

	seleccion = recibirSeleccionOperacion(socket);
	switch (seleccion) {
	case INICIAR_PROGRAMA:
		pid = recibirTamanio(socket);
		paginas = recibirTamanio(socket);
		rtaFuncion = inicializarPrograma(pid, paginas);
		sleep(getRetardo());
		enviarTamanio(socket, rtaFuncion);
		break;

	case SOLICITAR_BYTES_PAG:

		pid = recibirTamanio(socket);
		nroFrame = recibirTamanio(socket);
		offset = recibirTamanio(socket);
		size = recibirTamanio(socket);
		buffer = malloc(size);
		solicitarBytesDePagina(pid, nroFrame, offset, size);
		sleep(getRetardo());
		enviarMensaje(socket, buffer, size);
		free(buffer);
		break;
	case ALMACENAR_BYTES_PAG:

		buffer = malloc(size);
		pid = recibirTamanio(socket);
		nroFrame = recibirTamanio(socket);
		offset = recibirTamanio(socket);
		size = recibirTamanio(socket);
		recibirDatos(socket, &buffer, size);
		rtaFuncion = almacenarBytesEnPagina(pid, nroFrame, offset, size,
				buffer);
		sleep(getRetardo());
		enviarTamanio(socket, rtaFuncion);
		free(buffer);
		break;

	case ASIGNAR_PAGINAS_PRC:
		pid = recibirTamanio(socket);
		paginas = recibirTamanio(socket);
		rtaFuncion = asignarPaginasProceso(pid, paginas);
		sleep(getRetardo());
		enviarTamanio(socket, rtaFuncion);
		break;

	case FINALIZAR_PRG:
		pid = recibirTamanio(socket);
		rtaFuncion=finalizarPrograma(pid);
		sleep(getRetardo());
		enviarTamanio(socket, rtaFuncion);
		break;
	default:
		rtaFuncion=4040;
		printf("Error en la operacion de entrada");
		enviarTamanio(socket,rtaFuncion);
		break;

	}

}

/*int crearSocket() {
	//Declaracion de variable para recibir valor de rta
	int valorRtaSetSockOpt;
	int sockDeEspera;

	//Socket(): creates an endpoint for communication and returns a file descriptor that refers to that endpoint.

	sockDeEspera = socket(AF_INET, SOCK_STREAM, 0);
	errorSalidaSocket(sockDeEspera, "Fallo en la creacion del socket de direccionEspera",
			&sockDeEspera);

	valorRtaSetSockOpt = setsockopt(sockDeEspera, SOL_SOCKET, SO_REUSEADDR,
			(const char*) &yes, sizeof(yes));
	errorSalidaSocket(valorRtaSetSockOpt,
			"Error en setSockOpt en el socket de direccionEspera", &sockDeEspera);
	return sockDeEspera;
}
*/
