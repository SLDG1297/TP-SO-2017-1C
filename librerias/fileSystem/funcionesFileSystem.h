/*
 * funcionesFileSystem.h
 *
 *  Created on: 10/7/2017
 *      Author: utnso
 */

#ifndef FILESYSTEM_FUNCIONESFILESYSTEM_H_
#define FILESYSTEM_FUNCIONESFILESYSTEM_H_

#include "../serializador.h"

// Protocolo de comuniación del File System

#define VALIDAR_ARCHIVO			1701
#define CREAR_ARCHIVO			1702
#define BORRAR					1703
#define OBTENER_DATOS			1704
#define GUARDAR_DATOS			1705



// Variables globales

int socketKernel;		// Socket del Kernel.

char* puntoMontaje;		// Directorio raíz del File System.



// Declaraciones

// Funciones de conexión

void 	handshakeKernel();										// TODO: Handshake con el Kernel

void 	elegirOperacion(int accion);							// TODO: Selector de acciones del File System

// Funciones del File System

void	validarArchivo(char* path);								// TODO: Validar que el archivo solicitado exista en los directorios

void	crearArchivo(char* path);								// TODO: Crear un archivo nuevo

void	borrar(char* path);										// TODO: Borrar un archivo existente

void	obtenerDatos(char* path);								// TODO: Leer un archivo del File System

void	guardarDatos(char* path);								// TODO: Escribir datos en un archivo

// Funciones auxiliares

void	envolver(int _socketKernel, char* _puntoMontaje);		// Importa variables de file_system.c a funcionesFileSystem.h

void	iniciarEstructuras();									// Crea estructuras como Metadata, Bitmap y otras yerbes del FS.

void	iniciarMetadata();										//

void	iniciarBitmap();

void	iniciarFileMetadata();

void	iniciarDatos();



// Definiciones

void elegirOperacion(int accion){
	char* path;														// Directorio que envía el Kernel.

	recibirPaqueteVariable(socketKernel, (void**) path);			// Recepción del directorio que envía el Kernel.

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

	// Recorrer el directorio de archivos y corroborar si es una dirección válida.

	// Si se encuentra, se puede abrir el archivo. Caso contrario, avisar que no existe.
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

void iniciarEstructuras(){
	iniciarMetadata();

	iniciarBitmap();

	iniciarFileMetadata();

	niciarDatos();
}

void iniciarMetadata(){

}

void envolver(int _socketKernel, char* _puntoMontaje){
	socketKernel = _socketKernel;
	puntoMontaje = _puntoMontaje;
}

#endif /* FILESYSTEM_FUNCIONESFILESYSTEM_H_ */
