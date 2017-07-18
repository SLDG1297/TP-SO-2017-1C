#ifndef FILESYSTEM_SADICA_H_
#define FILESYSTEM_SADICA_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <commons/bitarray.h>
#include <commons/config.h>
#include <commons/txt.h>

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

void				iniciarMetadata(char* pathRaiz);						// Cargar el archivo Metadata.bin con los registros correspondientes.

registroMetadata	crearRegistroMetadata(t_config* configuracion);			// Crear estructuras para determinar tamaño y cantidad de bloques en FS.

registroMetadata	leerMetadata(char* pathRaiz);							// Obtener registro de Metadata.bin

void				cerrarMetadata(FILE* archivoMetadata);					// Cerrar el archivo Metadata.bin.

// Bitmap

char* 				crearRegistroBitmap();									// Crear estructuras para determinar los bloques ocupados en el FS.

FILE*				iniciarBitmap(char* pathRaiz);							// Crear bitmap con los bloques libres y ocupados en el FS.

t_bitarray*			leerBitmap(FILE* archivoBitmap);						// Obtener registro de Bitmap.bin.

// Otros

FILE*				iniciarArchivo(char* pathRaiz, char* pathRelativo);		// Abre un archivo simplificado.

char*				obtenerPath(char* pathRaiz, char* pathRelativo);		// Obtiene directorio de un archivo en el FS.

void				iniciarDirectorios(char* pathRaiz);						// Crear los directorios del FS.



// Definiciones

void iniciarMetadata(char* pathRaiz){
	char* pathMetadata = obtenerPath(pathRaiz, "/Metadata/Metadata.bin");

	char* tamanio = string_itoa(TAMANIO_BLOQUES);
	char* cantidad = string_itoa(CANTIDAD_BLOQUES);
	char* magica = "SADICA";

	FILE* archivoMetadata = txt_open_for_append(pathMetadata);

	t_config* configuracionMetadata = config_create(pathMetadata);

	config_set_value(configuracionMetadata, "TAMANIO_BLOQUES", tamanio);
	config_set_value(configuracionMetadata, "CANTIDAD_BLOQUES", cantidad);
	config_set_value(configuracionMetadata, "MAGIC_NUMBER", magica);

	config_save(configuracionMetadata);

	config_destroy(configuracionMetadata);

	txt_close_file(archivoMetadata);

	free(pathMetadata);

	free(tamanio);
	free(cantidad);
}

registroMetadata crearRegistroMetadata(t_config* configuracion){
	registroMetadata registro;

	registro.tamanioBloques = config_get_int_value(configuracion, "TAMANIO_BLOQUES");
	registro.cantidadBloques = config_get_int_value(configuracion, "CANTIDAD_BLOQUES");
	registro.numeroMagico = config_get_string_value(configuracion, "MAGIC_NUMBER");

	return registro;
}

registroMetadata leerMetadata(char* pathRaiz){
	char* pathMetadata = obtenerPath(pathRaiz, "/Metadata/Metadata.bin");

	t_config* configuracionMetadata = config_create(pathMetadata);

	registroMetadata registro = crearRegistroMetadata(configuracionMetadata);

	config_destroy(configuracionMetadata);

	free(pathMetadata);

	return registro;
}

char* crearRegistroBitmap(){
	char* bitarray = malloc(CANTIDAD_BLOQUES_BITS);		// Array de bitmaps.
	bzero(bitarray, CANTIDAD_BLOQUES_BITS);				// Setear en 0.

	return bitarray;
}

FILE* iniciarBitmap(char* pathRaiz){
	char* bitarray = crearRegistroBitmap();										// Array de bits en 0 para indicar que todos los bloques están libres en primera instancia.

	FILE* archivoBitmap = iniciarArchivo(pathRaiz, "/Metadata/Bitmap.bin");		// Abrir archivo Bitmap.bin.

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
	char* path = obtenerPath(pathRaiz, pathRelativo);

	FILE* archivo = fopen(path, "wb+");		// Abrir archivo.

	free(path);

	return archivo;
}

char* obtenerPath(char* pathRaiz, char* pathRelativo){
	char* path = string_new();				// Path real.

	string_append(&path, pathRaiz);
	string_append(&path, pathRelativo);		// Obtener path real.

	return path;
}

void iniciarDirectorios(char* pathRaiz){
	mkdir(pathRaiz, S_IRWXU);		// Creo el directorio de montaje.
	chdir(pathRaiz);				// Me muevo al directorio del FS.

	mkdir("Metadata", S_IRWXU);		// Creo los directorios que pide el enunciado.
	mkdir("Archivos", S_IRWXU);
	mkdir("Bloques", S_IRWXU);

	chdir("..");					// Vuelvo al directorio raíz del FS.
}

#endif /* FILESYSTEM_SADICA_H_ */
