#ifndef FILESYSTEM_SADICA_H_
#define FILESYSTEM_SADICA_H_

#include <fcntl.h>
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



// Estructuras de datos

typedef struct{
	u_int32_t 	tamanioBloques;				// Tamanio de los bloques que maneja el FS.
	u_int32_t 	cantidadBloques;			// Cantidad de bloques que maneja el FS.
	char* 		numeroMagico;				// Ni idea
} registroMetadata;



// Variables globales

registroMetadata	metadata;				// Información acerca de la cantidad y tamaño de bloques del FS.

// Bitmap

int					fdBitmap;				// Descriptor de archivo para Bitmap.bin

void*				mapaDeBits;				// Mapa de bits con los bloques libres y ocupados en el FS.

t_bitarray*			arrayDeBits;			// Estructura para acceder al mapa de bits.



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

int					obtenerBitmapFD();

void				iniciarBitmapFD(char* pathRaiz);

void				iniciarBitmapMap(char* pathRaiz);

void		 		leerBitmapMap();



// Otros (Ordenados alfabéticamente)

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



// Bitmap Viejo

char* iniciarRegistroBitmap(size_t tamanio){
	char* bitarray = malloc(tamanio);		// Array de bitmaps.
	bzero(bitarray, tamanio);				// Setear en 0.

	return bitarray;
}



// Bitmap

int obtenerBitmapFD(){
	return fdBitmap;
}

void iniciarBitmapFD(char* pathRaiz){
	fdBitmap = iniciarDescriptorArchivo(pathRaiz, "/Metadata/Bitmap.bin");		// Abrir archivo Bitmap.bin.
}

void cerrarBitmapFD(){
	close(fdBitmap);
}

void iniciarBitmap(char* pathRaiz){
	char* bitarray = iniciarRegistroBitmap(bloquesEnBytes());					// Array de bits en 0 para indicar que todos los bloques están libres en primera instancia.

	write(fdBitmap, bitarray, bloquesEnBytes());

	free(bitarray);
}

void abrirBitmap(char* pathRaiz){
	struct stat atributosBitmap;

	fstat(fdBitmap, &atributosBitmap);

	mapaDeBits = mmap(NULL, atributosBitmap.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdBitmap, 0);

	msync(mapaDeBits, bloquesEnBytes(), MS_SYNC);
}

void leerBitmap(){
	arrayDeBits = bitarray_create_with_mode(mapaDeBits, bloquesEnBytes(), LSB_FIRST);
}

t_bitarray* obtenerBitmap(){
	return arrayDeBits;
}

void cerrarBitmap(){
	munmap(mapaDeBits, bloquesEnBytes());

	bitarray_destroy(arrayDeBits);
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

FILE* iniciarArchivo(char* pathRaiz, char* pathRelativo){
	char* path = obtenerPath(pathRaiz, pathRelativo);

	FILE* archivo = fopen(path, "wb+");		// Abrir archivo.

	free(path);

	return archivo;
}

int iniciarDescriptorArchivo(char* pathRaiz, char* pathRelativo){
	char* path = obtenerPath(pathRaiz, pathRelativo);

	int fd = open(path, O_RDWR);		// Abrir archivo.

	free(path);

	return fd;
}

void iniciarDirectorios(char* pathRaiz){
	mkdir(pathRaiz, S_IRWXU);		// Creo el directorio de montaje.
	chdir(pathRaiz);				// Me muevo al directorio del FS.

	// Creo los directorios que pide el enunciado.

	crearArchivoConDirectorio("Metadata", "/Metadata.bin");
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
