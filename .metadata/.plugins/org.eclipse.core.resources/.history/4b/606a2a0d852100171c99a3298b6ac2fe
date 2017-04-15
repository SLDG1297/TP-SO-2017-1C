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

//La funcion busqueda clave int busca, a partir de una clave string, pasada por parametro, en el archivo
//kernel config el dato de tipo int al cual hace referencia esa clave.
int busquedaClaveInt (t_config* configuracion, char* palabraClave){
	int auxiliarDeClave=0;
	if(config_has_property(configuracion, palabraClave)){
		auxiliarDeClave = config_get_int_value(configuracion, palabraClave);}
		else{
			perror("No se encontró la palabra clave.");}
	return auxiliarDeClave;
}
//La funcion busqueda clave string busca, a partir de una clave string, pasada por parametro, en el archivo
//kernel config el dato de tipo string al cual hace referencia esa clave.
char* busquedaClaveChar (t_config* configuracion, char* palabraClave){
	int auxiliarDeClave=0;
	if(config_has_property(configuracion, palabraClave)){
		auxiliarDeClave = config_get_string_value(configuracion, palabraClave);}
		else{
			perror("No se encontró la palabra clave.");}
	return auxiliarDeClave;
}



int main(int argc, char *argv[])
{

	//Estructura para manejar el archivo de configuración -- t_config*
	//Crear estructura de configuración para obtener los datos del archivo de configuración.
	t_config* configuracion;
	configuracion = llamarArchivo();

	//Obtener IP del Kernel del archivo de configuración y chequear que sea correcto.
	int	PUERTO_PROG = busquedaClaveInt(configuracion,"PUERTO_PROG");


	int PUERTO_CPU = busquedaClaveInt(configuracion,"PUERTO_CPU");
	int PUERTO_MEMORIA = busquedaClaveInt(configuracion,"PUERTO_MEMORIA");
	int PUERTO_FS = busquedaClaveInt(configuracion,"PUERTO_FS");
	int QUANTUM = busquedaClaveInt(configuracion,"QUANTUM");
	int QUANTUM_SLEEP = busquedaClaveInt(configuracion,"QUANTUM_SLEEP");
	int GRADO_MULTIPROG= busquedaClaveInt(configuracion,"GRADO_MULTIPROG");
	int SEM_INIT [3];
	int STACK_SIZE= busquedaClaveInt(configuracion,"STACK_SIZE");

	char* IP_MEMORIA = busquedaClaveChar(configuracion,"IP_MEMORIA");
	char* IP_FS = busquedaClaveChar(configuracion,"IP_FS");
	char* ALGORITMO = busquedaClaveChar(configuracion, "ALGORITMO");
	char* SEM_IDS [3];
	char* SHARED_VARS[3];


	return 0;
}

