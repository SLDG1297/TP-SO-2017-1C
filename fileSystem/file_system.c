/*
 ============================================================================
 Name        : file_system.c
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
#include "../librerias/conexionSocket.h"
#include "../librerias/fileSystem/funcionesFileSystem.h"
#include "../librerias/fileSystem/sadica.h"
#include "../librerias/serializador.h"

#define RUTA_ARCHIVO "./config_file_system.cfg"



int main(){

	// Leer a archivo de configuración

	t_config* configuracion;
	char* ruta = RUTA_ARCHIVO;
	configuracion = llamarArchivo(ruta);

	int puertoKernel = busquedaClaveNumerica(configuracion, "PUERTO");
	char* puntoMontaje = busquedaClaveAlfanumerica(configuracion, "PUNTO_MONTAJE");

	envolver(puertoKernel, puntoMontaje);



	// Establecer conexión como servidor

	int socketKernel = servir(puertoKernel);



	// Elegir acción

	while(1)
	{
		u_int32_t accion = recibirHeader(socketKernel);

		elegirOperacion(accion);
	}

	return 0;

}
