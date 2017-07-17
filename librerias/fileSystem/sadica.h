#ifndef FILESYSTEM_SADICA_H_
#define FILESYSTEM_SADICA_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/mman.h>

#include <commons/bitarray.h>

#include "../serializador.h"



// Constantes

#define TAMANIO_BLOQUES			64
#define CANTIDAD_BLOQUES		5192
#define CANTIDAD_BLOQUES_BITS	CANTIDAD_BLOQUES / 8
#define TAMANIO_METADATA		2 * sizeof(u_int32_t) + strlen("SADICA") + 1



// Estructuras de datos

typedef struct{
	u_int32_t 	tamanioBloques;		// Tamanio de los bloques que maneja el FS.
	u_int32_t 	cantidadBloques;	// Cantidad de bloques que maneja el FS.
	char* 		numeroMagico;		// Ni idea
} registroMetadata;

// Registro que compone al archivo Metadata.



// Declaraciones

// Metadata

registroMetadata	crearRegistroMetadata();								// Crear estructuras para determinar tamaño y cantidad de bloques en FS.

FILE*				iniciarMetadata(char* pathRaiz);						// Cargar el archivo Metadata.bin con los registros correspondientes.

registroMetadata	leerMetadata(FILE* archivoMetadata);					// Obtener registro de Metadata.bin

void				cerrarMetadata(FILE* archivoMetadata);					// Cerrar el archivo Metadata.bin.

// Bitmap

char* 				crearRegistroBitmap();									// Crear estructuras para determinar los bloques ocupados en el FS.

FILE*				iniciarBitmap(char* pathRaiz);							// Crear bitmap con los bloques libres y ocupados en el FS.

t_bitarray*			leerBitmap(FILE* archivoBitmap);						// Obtener registro de Bitmap.bin.

void 				cerrarBitmap(FILE* archivoBitmap, t_bitarray* bitmap);	// Cerrar el archivo Bitmap.bin.

// Otros

FILE*				iniciarArchivo(char* pathRaiz, char* pathRelativo);		// Abre un archivo simplificado.



// Definiciones

registroMetadata crearRegistroMetadata(){
	registroMetadata registro;

	registro.tamanioBloques = TAMANIO_BLOQUES;
	registro.cantidadBloques = CANTIDAD_BLOQUES;
	registro.numeroMagico = "SADICA";

	return registro;
}

FILE* iniciarMetadata(char* pathRaiz){
	registroMetadata registro = crearRegistroMetadata();							// Registro de metadata.

	FILE* archivoMetadata = iniciarArchivo(pathRaiz, "Metadata/Metadata.bin");		// Abrir archivo de metadata.

	fwrite(&registro, TAMANIO_METADATA, 1, archivoMetadata);						// Escribir en archivo el registro con los datos del FS.

	return archivoMetadata;
}

registroMetadata leerMetadata(FILE* archivoMetadata){
	registroMetadata registro;									// Registro

	fread(&registro, TAMANIO_METADATA, 1, archivoMetadata);		// Leer archivo y obtener datos.

	return registro;
}

void cerrarMetadata(FILE* archivoMetadata){
	fclose(archivoMetadata);
}

char* crearRegistroBitmap(){
	char* bitarray = malloc(CANTIDAD_BLOQUES_BITS);		// Array de bitmaps.
	bzero(bitarray, CANTIDAD_BLOQUES_BITS);				// Setear en 0.

	return bitarray;
}

FILE* iniciarBitmap(char* pathRaiz){
	char* bitarray = crearRegistroBitmap();										// Array de bits en 0 para indicar que todos los bloques están libres en primera instancia.

	FILE* archivoBitmap = iniciarArchivo(pathRaiz, "Metadata/Bitmap.bin");		// Abrir archivo Bitmap.bin.

	fwrite(bitarray, sizeof(char), CANTIDAD_BLOQUES_BITS, archivoBitmap);

	free(bitarray);

	return archivoBitmap;
}

t_bitarray* leerBitmap(FILE* archivoBitmap){
	char* bitarray;																		// Bitmap con espacios libres y ocupados en FS.
	t_bitarray* bitmap;																	// Estructura para abstraer la lectura del bitmap.

	fread(bitarray, sizeof(char), CANTIDAD_BLOQUES_BITS, archivoBitmap);				// Leer de archivo el mapa de bits.

	bitmap = bitarray_create_with_mode(bitarray, CANTIDAD_BLOQUES_BITS, LSB_FIRST);		// Asignar estructura de abstraccion.

	return bitmap;
}

void cerrarBitmap(FILE* archivoBitmap, t_bitarray* bitmap){
	fclose(archivoBitmap);

	bitarray_destroy(bitmap);
}

FILE* iniciarArchivo(char* pathRaiz, char* pathRelativo){
	char* path = string_new();						// Path real.

	string_append(&path, pathRaiz);
	string_append(&path, pathRelativo);				// Obtener path real.

	FILE* archivo = fopen(path, "wb+");				// Abrir archivo.

	free(path);

	return archivo;
}

#endif /* FILESYSTEM_SADICA_H_ */
