/*
 * cpu.c
 *
 *  Created on: 29/4/2017
 *      Author: utnso
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
#define RUTA_ARCHIVO "./config_consola.cfg"
#define SIZE_DATA 1024
#define codigoCpu "1"

int main(int argc, char *argv[]) {

//CODIGO PARA LLAMAR AL ARCHIVO

//Estructura para manejar el archivo de configuración -- t_config*
//Crear estructura de configuración para obtener los datos del archivo de configuración.

	/*t_config* configuracion;
	char* ruta = RUTA_ARCHIVO;
	configuracion = llamarArchivo(ruta);*/

//DECLARACION DE VARIABLES PARA LA CONEXION AL KERNEL Y A LA MEMORIA

	int sockKernel, sockMemoria, bytesRecibidosKernel, bytesRecibidosMemoria;
	char datosEnviar[SIZE_DATA], datosRecibir[SIZE_DATA];
	memset(datosEnviar, '\0', SIZE_DATA);
	memset(datosRecibir, '\0', SIZE_DATA);
	struct sockaddr_in kernel_dir, memoria_dir;

//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION

	//*Obtener IP del Kernel del archivo de configuración y chequear que sea correcto.
	char* IP_KERNEL = "127.0.0.1";//busquedaClaveAlfanumerica(configuracion, "IP_KERNEL");
	char* IP_MEMORIA = "127.0.0.1";//busquedaClaveAlfanumerica(configuracion, "IP_MEMORIA");

	//*Obtener el puerto de Kernel y de la memoria del archivo de configuración y chequear que sea correcto.
	int PUERTO_KERNEL = 6000;//busquedaClaveNumerica(configuracion, "PUERTO_KERNEL");
	int PUERTO_MEMORIA = 5020;//busquedaClaveNumerica(configuracion, "PUERTO_MEMORIA");

//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

	int valorRtaConnect = 0;

// CODIGO PRINCIPAL DE LA CPU

	// host = gethostbyname(argv[1]);

	//*Se crea el socket para conectarse con el kernel y la memoria

	sockKernel = socket(AF_INET, SOCK_STREAM, 0);
	esErrorConSalida(sockKernel, "Error en el Socket");
	sockMemoria = socket(AF_INET, SOCK_STREAM, 0);
    esErrorConSalida(sockMemoria, "Error en el Socket");

	//ASIGNACION DE DATOS DEL KERNEL

	//*Direccion del servidor - KERNEL
	kernel_dir.sin_family = AF_INET;
	kernel_dir.sin_port = htons(PUERTO_KERNEL);
	kernel_dir.sin_addr.s_addr = inet_addr(IP_KERNEL);
	bzero(&(kernel_dir.sin_zero), 8);

	//ASIGNACION DE DATOS DEL KERNEL

	//*Direccion del servidor - MEMORIA
	memoria_dir.sin_family = AF_INET;
	memoria_dir.sin_port = htons(PUERTO_MEMORIA);
	memoria_dir.sin_addr.s_addr = inet_addr(IP_MEMORIA);
	bzero(&(kernel_dir.sin_zero), 8);

	//CONEXION A KERNEL

	valorRtaConnect = connect(sockKernel, (struct sockaddr *) &kernel_dir, sizeof(struct sockaddr));
	esErrorConSalida(valorRtaConnect, "Error en el connect");

	//*La cpu espera un mensaje de parte del kernel (handshake)

	bytesRecibidosKernel = recv(sockKernel, datosRecibir, SIZE_DATA, 0);
	datosRecibir[bytesRecibidosKernel] = '\0';

	//CONEXION A MEMORIA
	valorRtaConnect = connect(sockMemoria, (struct sockaddr *) &memoria_dir,sizeof(struct sockaddr));
	esErrorConSalida(valorRtaConnect, "Error en el connect");

	//*La cpu espera un mensaje de parte de la memoria (handshake)

	bytesRecibidosMemoria = recv(sockMemoria, datosRecibir, SIZE_DATA, 0);
	datosRecibir[bytesRecibidosMemoria] = '\0';

	while (1) {
		printf("\nDatos a enviar: ");
		gets(datosEnviar);
		send(sockKernel, datosEnviar, strlen(datosEnviar), 0);
		memset(datosEnviar, '\0', 1024);
	}

	return 0;
}
