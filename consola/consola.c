/*
 * configConsola.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
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
//#include <funcionesDeBusquedaClave.h>

#define RUTA_ARCHIVO "./config_consola.cfg"

// llamarArchivo(): Genera una estructura de datos de configuración.
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
	char* IP_KERNEL = busquedaClaveAlfanumerica(configuracion, "IP_KERNEL");

	//Obtener el puerto de Kernel del archivo de configuración y chaquear que sea correcto.
	int PUERTO_KERNEL = busquedaClaveNumerica(configuracion, "PUERTO_KERNEL");



	return 0;
}
