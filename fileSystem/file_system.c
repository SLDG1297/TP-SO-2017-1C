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

#define RUTA_ARCHIVO "./config_file_system.cfg"

t_config* llamarArchivo() {

	//Dirección para manejar el archivo de configuración.
	char* directorio;

	//Asignar ruta de acceso al archivo de configuración.
	directorio = RUTA_ARCHIVO;
	return config_create(directorio);
}

int busquedaClaveNumerica(t_config* configuracion, char* palabraClave) {
	int clave;
	if (config_has_property(configuracion, palabraClave))
		clave = config_get_int_value(configuracion, palabraClave);
	else
		perror("No se encontró.");
	return clave;
}

char* busquedaClaveAlfanumerica(t_config* configuracion, char* palabraClave) {
	char* clave;
	if (config_has_property(configuracion, palabraClave))
		clave = config_get_string_value(configuracion, palabraClave);
	else
		perror("No se encontró.");
	return clave;
}

int main(int argc, char *argv[]) {

	//Estructura para manejar el archivo de configuración -- t_config*
	//Crear estructura de configuración para obtener los datos del archivo de configuración.
	t_config* configuracion;
	configuracion = llamarArchivo();

	int PUERTO = busquedaClaveNumerica(configuracion, "PUERTO");
	char* PUNTO_MONTAJE = busquedaClaveAlfanumerica(configuracion,"PUNTO_MONTAJE");


	return 0;
}
