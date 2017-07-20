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


typedef struct{
	u_int32_t 	tamanioBloques;				// Tamanio de los bloques que maneja el FS.
	u_int32_t 	cantidadBloques;			// Cantidad de bloques que maneja el FS.
	char* 		numeroMagico;				// Ni idea
} registroMetadata;

// Variables globales

registroMetadata	metadata;				// Información acerca de la cantidad y tamaño de bloques del FS.

// Bitmap

int					fdBitmap;				// Descriptor de archivo para Bitmap.bin

char*				bitarray;				// Mapa de bits con los bloques libres y ocupados en el FS.



// Declaraciones

// Metadata

	// Interfaz

void				iniciarMetadata(char* pathRaiz);

void				leerMetadata(char* pathRaiz);

void				escribirMetadata(char* pathRaiz);

void				escribirMetadata(char* pathRaiz);

	// Operaciones internas

void 				plantillaMetadata(char* pathRaiz, void(*procedimiento)(t_config*));		// Plantilla para no repetir código en los procedimientos para asignar Metadata.bin.

void				operacionIniciarMetadata(t_config* configuracionMetadata);				// Cargar el archivo Metadata.bin con los registros correspondientes.

void 				operacionLeerMetadata(t_config* configuracionMetadata);					// Obtener valores del archivo de metadata.

void				operacionEscribirMetadata(t_config* configuracionMetadata);				// Cambiar los valores del archivo de metadata.

registroMetadata	creadorMetadata(u_int32_t tamanio, u_int32_t cantidad, char* magica);	// Builder.

void				asignarMetadata(u_int32_t tamanio, u_int32_t cantidad, char* magica);	// Setter.

registroMetadata 	devolverMetadata();														// Getter.

void 				destruirMetadata(registroMetadata registro);							// Para liberar la memoria del registro metadata.

// Bitmap

char* 				crearRegistroBitmap(size_t tamanio);					// Crear estructuras para determinar los bloques ocupados en el FS.

FILE*				iniciarBitmap(char* pathRaiz);							// Crear bitmap con los bloques libres y ocupados en el FS.

t_bitarray*			leerBitmap(FILE* archivoBitmap);						// Obtener registro de Bitmap.bin.

// TODO: Con mapeo

int iniciarBitmapMap(char* pathRaiz);

t_bitarray* leerBitmapMap(char* pathMetadata);

// Otros

u_int32_t			bloquesEnBytes();													// Devuelve la cantidad de bytes que ocupa el mapa de bits.

void				crearArchivoConDirectorio(char* directorio, char* pathRelativo);

void				destruirBitarray(t_bitarray *bitarray);

FILE*				iniciarArchivo(char* pathRaiz, char* pathRelativo);					// Abre un archivo simplificado.

int					iniciarDescriptorArchivo(char* pathRaiz, char* pathRelativo);		// Abre un archivo y devuelve su descriptor de archivo.

void				iniciarDirectorios(char* pathRaiz);									// Crear los directorios del FS.

char*				obtenerPath(char* pathRaiz, char* pathRelativo);					// Obtiene directorio de un archivo en el FS.



// Definiciones

void iniciarMetadata(char* pathRaiz){
	plantillaMetadata(pathRaiz, &operacionIniciarMetadata);
}

void leerMetadata(char* pathRaiz){
	plantillaMetadata(pathRaiz, &operacionLeerMetadata);
}

void escribirMetadata(char* pathRaiz){
	plantillaMetadata(pathRaiz, &operacionEscribirMetadata);
}

void operacionIniciarMetadata(t_config* configuracionMetadata){
	asignarMetadata(TAMANIO_BLOQUES, CANTIDAD_BLOQUES, "SADICA");

	operacionEscribirMetadata(configuracionMetadata);
}

void operacionLeerMetadata(t_config* configuracionMetadata){
	metadata.tamanioBloques = config_get_int_value(configuracionMetadata, "TAMANIO_BLOQUES");
	metadata.cantidadBloques = config_get_int_value(configuracionMetadata, "CANTIDAD_BLOQUES");
	metadata.numeroMagico = string_duplicate(config_get_string_value(configuracionMetadata, "MAGIC_NUMBER"));
}

void operacionEscribirMetadata(t_config* configuracionMetadata){
	char* tamanio = string_itoa(metadata.tamanioBloques);
	char* cantidad = string_itoa(metadata.cantidadBloques);
	char* magica = "SADICA";

	config_set_value(configuracionMetadata, "TAMANIO_BLOQUES", tamanio);
	config_set_value(configuracionMetadata, "CANTIDAD_BLOQUES", cantidad);
	config_set_value(configuracionMetadata, "MAGIC_NUMBER", magica);

	config_save(configuracionMetadata);

	free(tamanio);
	free(cantidad);
}

registroMetadata creadorMetadata(u_int32_t tamanio, u_int32_t cantidad, char* magica){
	registroMetadata nuevo;

	nuevo.tamanioBloques = tamanio;
	nuevo.cantidadBloques = cantidad;
	nuevo.numeroMagico = magica;

	return nuevo;
}

void asignarMetadata(u_int32_t tamanio, u_int32_t cantidad, char* magica){
	metadata.tamanioBloques = tamanio;
	metadata.cantidadBloques = cantidad;
	metadata.numeroMagico = magica;
}

registroMetadata devolverMetadata(){
	return metadata;
}

void destruirMetadata(registroMetadata registro){
	free(registro.numeroMagico);
}

void plantillaMetadata(char* pathRaiz, void(*procedimiento)(t_config*)){
	char* pathMetadata = obtenerPath(pathRaiz, "/Metadata/Metadata.bin");

	t_config* configuracionMetadata = config_create(pathMetadata);

	procedimiento(configuracionMetadata);

	config_destroy(configuracionMetadata);

	free(pathMetadata);
}



// Bitmap

char* crearRegistroBitmap(size_t tamanio){
	char* bitarray = malloc(tamanio);		// Array de bitmaps.
	bzero(bitarray, tamanio);				// Setear en 0.

	return bitarray;
}

FILE* iniciarBitmap(char* pathRaiz){
	char* bitarray = crearRegistroBitmap(bloquesEnBytes());								// Array de bits en 0 para indicar que todos los bloques están libres en primera instancia.

	FILE* archivoBitmap = iniciarArchivo(pathRaiz, "/Metadata/BitmapViejo.bin");		// Abrir archivo Bitmap.bin.

	fwrite(bitarray, sizeof(char), bloquesEnBytes(), archivoBitmap);

	free(bitarray);

	return archivoBitmap;
}

t_bitarray* leerBitmap(FILE* archivoBitmap){
	char* bitarray = crearRegistroBitmap(bloquesEnBytes());																	// Bitmap con espacios libres y ocupados en FS.
	t_bitarray* bitmap;																// Estructura para abstraer la lectura del bitmap.

	fread(bitarray, sizeof(char), bloquesEnBytes(), archivoBitmap);					// Leer de archivo el mapa de bits.

	bitmap = bitarray_create_with_mode(bitarray, bloquesEnBytes(), LSB_FIRST);		// Asignar estructura de abstraccion.

	return bitmap;
}

int iniciarBitmapMap(char* pathRaiz){
	char* bitarray = crearRegistroBitmap(bloquesEnBytes());						// Array de bits en 0 para indicar que todos los bloques están libres en primera instancia.

	FILE* archivoBitmap = iniciarArchivo(pathRaiz, "/Metadata/Bitmap.bin");		// Abrir archivo Bitmap.bin.

	fwrite(bitarray, sizeof(char), bloquesEnBytes(), archivoBitmap);

	free(bitarray);

	return fileno(archivoBitmap);
}

/*t_bitarray* leerBitmapMap(char* pathRaiz){
	char* bitarray = crearRegistroBitmap(bloquesEnBytes());																	// Bitmap con espacios libres y ocupados en FS.
	t_bitarray* bitmap;



	return bitmap;
}*/

void cerrarBitmap(FILE* archivoBitmap, t_bitarray* bitmap){
	fclose(archivoBitmap);

	destruirBitarray(bitmap);
}



// Otros

u_int32_t bloquesEnBytes(){
	return metadata.cantidadBloques / 8;
}

void crearArchivoConDirectorio(char* directorio, char* pathRelativo){
	mkdir(directorio, S_IRWXU);

	char* path = obtenerPath(directorio, pathRelativo);

	FILE* archivo = fopen(path, "wb+");

	fclose(archivo);

	free(path);
}

void destruirBitarray(t_bitarray *bitarray){
	free(bitarray->bitarray);
	free(bitarray);
}

FILE* iniciarArchivo(char* pathRaiz, char* pathRelativo){
	char* path = obtenerPath(pathRaiz, pathRelativo);

	FILE* archivo = fopen(path, "wb+");		// Abrir archivo.

	free(path);

	return archivo;
}

int iniciarDescriptorArchivo(char* pathRaiz, char* pathRelativo){
	char* path = obtenerPath(pathRaiz, pathRelativo);

	int fd = open(path, "wb+");				// Abrir archivo.

	free(path);

	return fd;
}

void iniciarDirectorios(char* pathRaiz){
	mkdir(pathRaiz, S_IRWXU);		// Creo el directorio de montaje.
	chdir(pathRaiz);				// Me muevo al directorio del FS.

	// Creo los directorios que pide el enunciado.

	crearArchivoConDirectorio("Metadata", "/Metadata.bin");
	crearArchivoConDirectorio("Metadata", "/BitmapViejo.bin");
	crearArchivoConDirectorio("Metadata", "/Bitmap.bin");

	mkdir("Archivos", S_IRWXU);		// TODO: Ver si falta alguno.
	mkdir("Bloques", S_IRWXU);		// TODO: Ver si falta alguno.

	chdir("..");					// Vuelvo al directorio raíz del FS.
}

char* obtenerPath(char* pathRaiz, char* pathRelativo){
	char* path = string_new();				// Path real.

	string_append(&path, pathRaiz);
	string_append(&path, pathRelativo);		// Obtener path real.

	return path;
}

#endif /* FILESYSTEM_SADICA_H_ */
