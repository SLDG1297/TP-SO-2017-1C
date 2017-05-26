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

#define RUTA_ARCHIVO "./config_consola.cfg"
#define SIZE_DATA 1024

int main(int argc, char *argv[]) {

//CODIGO PARA LLAMAR AL ARCHIVO

//Estructura para manejar el archivo de configuraci�n -- t_config*
//Crear estructura de configuraci�n para obtener los datos del archivo de configuraci�n.

	t_config* configuracion;
	char* ruta = RUTA_ARCHIVO;
	configuracion = llamarArchivo(ruta);

//DECLARACION DE VARIABLES PARA EL CODIGO PRINCIPAL

	int sock, bytesRecibidos, longitudBytesRecibidos;
	int i;
	char datosEnviar[SIZE_DATA], datosRecibir[SIZE_DATA], idPrograma[SIZE_DATA],buffer[1024];
	memset(datosEnviar, '\0', SIZE_DATA);
	memset(datosRecibir, '\0', SIZE_DATA);
	memset(idPrograma, '\0', SIZE_DATA);
	memset (buffer,'\0',SIZE_DATA);
	struct sockaddr_in kernel_dir;


//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION

	//*Obtener IP del Kernel del archivo de configuraci�n y chequear que sea correcto.
	char* IP_KERNEL = busquedaClaveAlfanumerica(configuracion, "IP_KERNEL");

	//*Obtener el puerto de Kernel del archivo de configuraci�n y chequear que sea correcto.
	int PUERTO_KERNEL = busquedaClaveNumerica(configuracion, "PUERTO_KERNEL");

//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

	int valorRtaConnect = 0;
	int valorRtaProgramaNuevo = 0;
	int valorRtaEnvioArchivo = 0;

// CODIGO PRINCIPAL DE LA CONSOLA

	// host = gethostbyname(argv[1]);


//MENU DE OPCIONES PARA EL USUARIO


//FUNCION QUE NO SIRVE - temporal solo para planear c�mo aplicar el thread   -EBB
		int contador = 0;

		void* doSomeThing(void *arg) {
	    unsigned long i = 0;
		contador += 1;
	    printf("\n Comenz� el programa %d \n", contador);

	    for(i=0; i<(1);i++);
	    printf("\n El programa %d termin�\n", contador);

	    return NULL;}


//EL USUARIO ELIJE CREAR UN NUEVO PROGRAMA


    /*Declaraci�n de variables para los threads */

	pthread_t progID[50];
	int numeroDePrograma = 0;
	int counter = 0;
	int flagFinPrograma = 9999; /* DECLARAR ALGO QUE SIGNIFIQUE FIN DE UN PROGRAMA    -EBB  */


	/*Creando hilo del programa   												CONFIGURACION PRE-COMIENZO DE UN PROGRAMA - PARTE 1 DE 5*/

	while(numeroDePrograma < 50)
	    {
		valorRtaProgramaNuevo = pthread_create(&(progID[numeroDePrograma]), NULL, &doSomeThing, NULL);
	        if (valorRtaProgramaNuevo != 0){
	            esErrorSinSalida(valorRtaProgramaNuevo,"Error en Send");
				}
	        else {numeroDePrograma++;}
	    }

	/*Funcion para que un thread lea su configuraci�n inicial   			CONFIGURACION PRE-COMIENZO DE UN PROGRAMA - PARTE 2 DE 5 */
	/*El thread se conecta mediante sockets al kernel   					CONFIGURACION PRE-COMIENZO DE UN PROGRAMA - PARTE 3 DE 5*/


	/*Se crea el socket para conectarse con el kernel*/

	sock = socket(AF_INET, SOCK_STREAM, 0);
	esErrorConSalida(sock, "Error en el Socket");

	//ASIGNACION DE DATOS DEL KERNEL

	//*Direccion del servidor - KERNEL
	kernel_dir.sin_family = AF_INET;
	kernel_dir.sin_port = htons(PUERTO_KERNEL);
	kernel_dir.sin_addr.s_addr = inet_addr(IP_KERNEL);
	bzero(&(kernel_dir.sin_zero), 8);

	//CONEXION A KERNEL

	valorRtaConnect = connect(sock, (struct sockaddr *) &kernel_dir,
			sizeof(struct sockaddr));
	esErrorConSalida(valorRtaConnect, "Error en el connect");

	//*El cliente espera un mensaje de parte del kernel (handshake)

	bytesRecibidos = recv(sock, datosRecibir, SIZE_DATA, 0);
	datosRecibir[bytesRecibidos] = '\0';




	/*Enviar programa ANSISOP al Kernel (s�lo la direccion)				   CONFIGURACION PRE-COMIENZO DE UN PROGRAMA - PARTE 4 DE 5*/

		/*enviar direccion de archivo */

		printf("\nIndique la ubicaci�n del archivo del programa:");
		gets(datosEnviar);
		valorRtaEnvioArchivo= send(sock, datosEnviar, strlen(datosEnviar), 0);
		memset(datosEnviar, '\0', 1024);
		esErrorSinSalida(valorRtaEnvioArchivo,"Error en Send");
		//poner un mutex??   -EBB



	/*Recibir del Kernel el PID del proceso 								  CONFIGURACION PRE-COMIENZO DE UN PROGRAMA - PARTE 5 DE 5*/

		longitudBytesRecibidos = recv(i, buffer, sizeof(buffer), 0);
		if (longitudBytesRecibidos <= 0){
			perror("Error al recibir el PID del proceso");
			}
		memset(idPrograma, '\0', 1024);





	/* El kernel env�a algo para imprimir por pantalla */
		longitudBytesRecibidos = recv(i, buffer, sizeof(buffer), 0);
		if (longitudBytesRecibidos <= 0){
			perror("Error al recibir el PID del proceso");
			}
		else if(buffer == flagFinPrograma){  /* Notificaci�n de fin de programa */
		printf("Fin de programa\n");
		pthread_exit(0);
		}
		memset(idPrograma, '\0', 1024);


	return 0;
}
