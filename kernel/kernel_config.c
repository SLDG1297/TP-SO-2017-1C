/*
 ============================================================================
 Name        : kernelConfig.c
 Author      : Zero Gravity
 Version     :
 Copyright   :
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
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

#define RUTA_ARCHIVO "./config_kernel.cfg"

t_config* llamarArchivo (){

	//Dirección para manejar el archivo de configuración.
	char* directorio;

	//Asignar ruta de acceso al archivo de configuración.
	directorio = RUTA_ARCHIVO ;

	return config_create(directorio);
}



// busquedaClaveNumerica(): Devuelve un dato numerico en función de una
// palabra clave que se encuentre en el archivo de configuración.
int busquedaClaveNumerica(t_config* configuracion, char* palabraClave) {
	int clave;
	if (config_has_property(configuracion, palabraClave))
		clave = config_get_int_value(configuracion, palabraClave);
	else
		perror("No se encontró");
	return clave;
}

// busquedaClaveAlfanumerica(): Devuelve un dato alfanumerico en función de una
// palabra clave que se encuentre en el archivo de configuración.
char* busquedaClaveAlfanumerica(t_config* configuracion, char* palabraClave) {
	char* clave;
	if (config_has_property(configuracion, palabraClave))
		clave = config_get_string_value(configuracion, palabraClave);
	else
		perror("No se encontró.");
	return clave;
}



int main(int argc, char *argv[])
{

	//Estructura para manejar el archivo de configuración -- t_config*
	//Crear estructura de configuración para obtener los datos del archivo de configuración.
	t_config* configuracion;
	configuracion = llamarArchivo();

	//Obtener IP del Kernel del archivo de configuración y chequear que sea correcto.
	int	PUERTO_PROG = busquedaClaveNumerica(configuracion,"PUERTO_PROG");
	int PUERTO_CPU = busquedaClaveNumerica(configuracion,"PUERTO_CPU");
	int PUERTO_MEMORIA = busquedaClaveNumerica(configuracion,"PUERTO_MEMORIA");
	int PUERTO_FS = busquedaClaveNumerica(configuracion,"PUERTO_FS");
	int QUANTUM = busquedaClaveNumerica(configuracion,"QUANTUM");
	int QUANTUM_SLEEP = busquedaClaveNumerica(configuracion,"QUANTUM_SLEEP");
	int GRADO_MULTIPROG= busquedaClaveNumerica(configuracion,"GRADO_MULTIPROG");
	int SEM_INIT [3];
	int STACK_SIZE= busquedaClaveNumerica(configuracion,"STACK_SIZE");

	char* IP_MEMORIA = busquedaClaveAlfanumerica(configuracion,"IP_MEMORIA");
	char* IP_FS = busquedaClaveAlfanumerica(configuracion,"IP_FS");
	char* ALGORITMO = busquedaClaveAlfanumerica(configuracion, "ALGORITMO");
	char* SEM_IDS [3];
	char* SHARED_VARS[3];


	return 0;
}

