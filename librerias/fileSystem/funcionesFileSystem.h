#ifndef FILESYSTEM_FUNCIONESFILESYSTEM_H_
#define FILESYSTEM_FUNCIONESFILESYSTEM_H_

#include <stdio.h>
#include <stdbool.h>

#include "../serializador.h"

// Protocolo de comuniación del File System

#define VALIDAR_ARCHIVO			1701
#define CREAR_ARCHIVO			1702
#define BORRAR					1703
#define OBTENER_DATOS			1704
#define GUARDAR_DATOS			1705

// Otros valores

#define TAMANIO_METADATA		2 * sizeof(u_int32_t) + strlen("SADICA") + 1



// Estructuras de datos

typedef struct{
	u_int32_t 	tamanioBloques;		// Tamanio de los bloques que maneja el FS.
	u_int32_t 	cantidadBloques;	// Cantidad de bloques que maneja el FS.
	char* 		numeroMagico;		// Ni idea
} registroMetadata;

// Registro que compone al archivo Metadata.



// Variables globales

int 	socketKernel;		// Socket del Kernel.

char* 	puntoMontaje;		// Directorio raíz del File System.



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

bool	validar(char* path);									// Devuelve true si existe el archivo y false en caso contrario.

void	iniciarMetadata(char* pathRaiz);						// Crear estructuras para determinar tamaño y cantidad de bloques en FS.

void	envioSimplificado(void* dato, u_int32_t tamanio);		// Ahorra el trabajo de empaquetar las cosas, que a veces puede ser denso.

void	envolver(int _socketKernel, char* _puntoMontaje);		// Importa variables de file_system.c a funcionesFileSystem.h



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

void iniciarMetadata(char* pathRaiz){
	char* path = string_new();

	string_append(&path, pathRaiz);
	string_append(&path, "Metadata/Metadata.bin");				// Obtener path real.

	FILE* metadata = fopen(path, "w");							// Abrir archivo de metadata.

	registroMetadata registro;

	registro.tamanioBloques = 64;		// Hardcodeado
	registro.cantidadBloques = 5192;	// Hardcodeado
	registro.numeroMagico = "SADICA";	// Hardcodeado

	fwrite(&registro, TAMANIO_METADATA, 1, metadata);			// Escribir en archivo el registro con los datos del FS.

	fclose(metadata);

	free(path);
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
