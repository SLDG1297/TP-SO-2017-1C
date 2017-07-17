#ifndef FILESYSTEM_FUNCIONESFILESYSTEM_H_
#define FILESYSTEM_FUNCIONESFILESYSTEM_H_

#include "sadica.h"
#include "../serializador.h"

#include <stdio.h>
#include <stdbool.h>

// Protocolo de comuniación del File System

#define VALIDAR_ARCHIVO			1701
#define CREAR_ARCHIVO			1702
#define BORRAR					1703
#define OBTENER_DATOS			1704
#define GUARDAR_DATOS			1705



// Variables globales

int 	socketKernel;		// Socket del Kernel.

char* 	puntoMontaje;		// Directorio raíz del File System.



// Declaraciones

// Funciones de conexión

void 	handshakeKernel();										// TODO: Handshake con el Kernel

void 	elegirOperacion(int accion);							// TODO: Selector de acciones del File System

// Funciones del File System

void	validarArchivo(char* path);								// Validar que el archivo solicitado exista en los directorios

void	crearArchivo(char* path);								// TODO: Crear un archivo nuevo

void	borrar(char* path);										// TODO: Borrar un archivo existente

void	obtenerDatos(char* path);								// TODO: Leer un archivo del File System

void	guardarDatos(char* path);								// TODO: Escribir datos en un archivo

// Funciones auxiliares

bool	validar(char* path);									// Devuelve true si existe el archivo y false en caso contrario.

void 	envioSimplificado(void* dato, u_int32_t tamanio);		// Simplifica el envío de paquetes serializados al Kernel.

void	envolver(int _socketKernel, char* _puntoMontaje);		// Importa las variables de la función principal.



// Definiciones

void elegirOperacion(int accion){
	char* pathRelativo;												// Directorio que envía el Kernel.

	recibirPaqueteVariable(socketKernel, (void**) pathRelativo);	// Recepción del directorio que envía el Kernel.

	char* path = strcat(puntoMontaje, pathRelativo);				// Concatena el path recibido con el del FS para generar la dirección real.

	switch(accion)													// Selección de acción del File System.
	{
		case VALIDAR_ARCHIVO	: validarArchivo(path);	break;
		case CREAR_ARCHIVO		: crearArchivo(path);	break;
		case BORRAR				: borrar(path);			break;
		case OBTENER_DATOS		: obtenerDatos(path);	break;
		case GUARDAR_DATOS		: guardarDatos(path);	break;
	}

}

void validarArchivo(char* path){
	bool verificador = validar(path);					// Indica si el archivo existe o no.

	envioSimplificado(&verificador, sizeof(bool));		// Se envía la confirmación al Kernel.
}

bool validar(char* path){
	bool verificador;						// Indica si el archivo existe o no.

	FILE* archivo = fopen(path, "r");		// Intenta abrir el archivo.

	if (archivo != NULL)
	{
		verificador = true;					// El archivo existe.
		fclose(archivo);					// Cerrar el archivo que se pudo encontrar.
	}

	else verificador = false;				// El archivo no existe.

	return verificador;
}

void crearArchivo(char* path){

	// Crear el archivo nuevo con el nombre del path y al menos un  bloque de datos.
}

void borrar(char* path){

	// Borrar el archivo con dicho path.
}

void obtenerDatos(char* path){

	// Envía al Kernel los datos dentro de ese path.
}

void guardarDatos(char* path){

	// Guarda datos en el archivo que indica el path.
}

void envioSimplificado(void* dato, u_int32_t tamanio){
	paquete* envio = crearPaquete(tamanio);

	empaquetar(envio, &dato, tamanio);

	enviarPaquete(socketKernel, envio);
}

void envolver(int _socketKernel, char* _puntoMontaje){
	socketKernel = _socketKernel;
	puntoMontaje = _puntoMontaje;
}

#endif /* FILESYSTEM_FUNCIONESFILESYSTEM_H_ */
