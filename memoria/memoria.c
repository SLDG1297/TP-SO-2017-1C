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

#define RUTA_ARCHIVO "./config_memoria.cfg"
#define SIZE_DATA 1024



int main(int argc, char *argv[]) {

//CODIGO PARA LLAMAR AL ARCHIVO

	//Estructura para manejar el archivo de configuración -- t_config*
	//Crear estructura de configuración para obtener los datos del archivo de configuración.

	t_config* configuracion;
	char* ruta = RUTA_ARCHIVO;
	configuracion = llamarArchivo(ruta);

//DECLARACION DE VARIABLES PARA EL CODIGO PRINCIPAL

	//*Socket para esperar conexion con el kernel
	 int sockDeEspera;
	 int longitudDatosEnviados;
	 int longitudEstructuraSocket;
	 int bytesRecibidos;

	 //*Socket para entablar conexion con el kernel
	 int sockAlKernel;
	 int yes = 1;
	 char buffer[SIZE_DATA];
	 char* handshake = "La conexion al proceso memoria fue exitosa";
	 memset (buffer,'\0',SIZE_DATA);
	 struct sockaddr_in espera,datosDelKernel;

//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION

	int PUERTO_MEMORIA = busquedaClaveNumerica(configuracion, "PUERTO");

	/*
	 int MARCOS = busquedaClaveNumerica(configuracion, "MARCOS");
	 int MARCO_SIZE = busquedaClaveNumerica(configuracion, "MARCO_SIZE");
	 int ENTRADAS_CACHE = busquedaClaveNumerica(configuracion, "ENTRADAS_CACHE");
	 int CACHE_X_PROC = busquedaClaveNumerica(configuracion, "CACHE_X_PROC");
	 int RETARDO_MEMORIA = busquedaClaveNumerica(configuracion, "RETARDO_MEMORIA");
	 */


//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

	int valorRtaSetSockOpt = 0;
	int valorRtaBind = 0;
	int valorRtaListen = 0;


// CODIGO PRINCIPAL DE LA CONSOLA


	 sockDeEspera = socket(AF_INET,SOCK_STREAM,0);
	 esErrorConSalida(sockDeEspera,"Fallo en la creacion del socket de espera");

	 valorRtaSetSockOpt = setsockopt(sockDeEspera, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	 esErrorConSalida(valorRtaSetSockOpt,"Error en setSockOpt en el socket de espera");

	 printf("Setsockopt correcto\n");
	 puts("Socket de espera creado\n");

	 //DIRECCION PARA ESPERAR A LA CONEXION CON EL KERNEL

	 espera.sin_family = AF_INET;
	 espera.sin_port = htons(PUERTO_MEMORIA);
	 espera.sin_addr.s_addr = INADDR_ANY;
	 bzero(&(espera.sin_zero), 8);

	 valorRtaBind = bind(sockDeEspera, (struct sockaddr *) &espera,sizeof(struct sockaddr));
	 esErrorConSalida(valorRtaBind,"Error en Bind");

	 printf("Bind correcto\n");
	 puts("Esperando la conexion del kernel\n");

	 valorRtaListen = listen(sockDeEspera,1);
	 esErrorConSalida(valorRtaListen,"Error en Listen");


	 longitudEstructuraSocket= sizeof (datosDelKernel);

	 sockAlKernel = accept(sockDeEspera,(struct sockaddr *) &datosDelKernel,&longitudEstructuraSocket);
	 esErrorConSalida(sockAlKernel,"Error en el accept");

	 close(sockDeEspera); //Se cierra el socket que se usaba para escuchar ya que no es relevante

	 longitudDatosEnviados = send(sockAlKernel,handshake, strlen(handshake),0);
	 esErrorSinSalida(longitudDatosEnviados,"Fallo en el handshake");

	 puts ("La conexion al proceso kernel fue exitosa\n");
	 puts ("Esperando mensajes\n");

	 while(1) {

		 bytesRecibidos = recv(sockAlKernel,buffer,sizeof(buffer),0);
		 esErrorSinSalida(bytesRecibidos,"Fallo en la recepcion");

		 sinBytesRecibidos(bytesRecibidos);

		 printf ("Buffer: %s\n",buffer);
		 memset (buffer,'\0',SIZE_DATA);
	 }


	 return 0;



	 }
