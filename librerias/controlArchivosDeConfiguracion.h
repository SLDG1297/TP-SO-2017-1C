/*
 * controlArchivosDeConfiguracion.h
 *
 *  Created on: 15/4/2017
 *      Author: utnso
 */

#ifndef CONTROLARCHIVOSDECONFIGURACION_H_
#define CONTROLARCHIVOSDECONFIGURACION_H_

#include <commons/config.h>
#include <stdbool.h>


t_config* llamarArchivo(char* RUTA_ARCHIVO) {

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

#endif /* CONTROLARCHIVOSDECONFIGURACION_H_ */
