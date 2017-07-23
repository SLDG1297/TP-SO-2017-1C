#ifndef FILESYSTEM_SADICA_H_
#define FILESYSTEM_SADICA_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <commons/bitarray.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/txt.h>

#include "../serializador.h"



// Constantes

#define TAMANIO_BLOQUES			64
#define CANTIDAD_BLOQUES		5192



// Estructuras de datos

typedef struct{
	u_int32_t 	tamanioBloques;				// Tamanio de los bloques que maneja el FS.
	u_int32_t 	cantidadBloques;			// Cantidad de bloques que maneja el FS.
	char* 		numeroMagico;				// Ni idea. Supestamente es un valor que se usa para distinguir algo. Pero ni idea.
}

registroMetadata;							// Indica la composición del FS.

typedef struct{
	u_int32_t	tamanioArchivo;				// Indica el tamaño total del archivo
	u_int32_t*	bloques;					// Bloques asingados al archivo.
}

registroArchivo;							// Indica la composición de un archivo



// Variables globales

// Punto de montaje

char*				puntoMontaje;			// Directorio raíz del FS.

// Metadata

registroMetadata	metadata;				// Información acerca de la cantidad y tamaño de bloques del FS.

// Bitmap

int					fdBitmap;				// Descriptor de archivo para Bitmap.bin

void*				mapaDeBits;				// Mapa de bits con los bloques libres y ocupados en el FS.

t_bitarray*			arrayDeBits;			// Estructura para acceder al mapa de bits.



// Declaraciones

// Metadata (Este me quedó Objetoso)

	// Interfaz

void				iniciarMetadata();		// Inicia el archivo Metadata.bin desde 0.

void				leerMetadata();			// Obtiene el tamaño de los bloques y la cantidad de bloques del FS del archivo Metadata.bin.

void				escribirMetadata();		// Cambia el contenido de Metadata.bin.

	// Operaciones internas

void 				plantillaMetadata(void(*procedimiento)(t_config*));						// Plantilla para no repetir código en los procedimientos para asignar Metadata.bin.

void				operacionIniciarMetadata(t_config* configuracionMetadata);				// Cargar el archivo Metadata.bin con los registros correspondientes.

void 				operacionLeerMetadata(t_config* configuracionMetadata);					// Obtener valores del archivo de metadata.

void				operacionEscribirMetadata(t_config* configuracionMetadata);				// Cambiar los valores del archivo de metadata.

registroMetadata	creadorMetadata(u_int32_t tamanio, u_int32_t cantidad, char* magica);	// Builder.

void				asignarMetadata(u_int32_t tamanio, u_int32_t cantidad, char* magica);	// Setter.

registroMetadata 	obtenerMetadata();														// Getter.



// Bitmap

void				iniciarBitmap();					// Inicializa el archivo Bitmap.bin con todos los bloques libres.

void				abrirBitmap();						// Aloca espacio en memoria para las estructuras de Bitmap.bin en memoria.

void				cerrarBitmap();						// Libera todas las estructuras de Bitmap.bin en memoria.

t_bitarray*			obtenerBitmap();					// Devuelve una instancia del mapa de bits.

void				ocuparBloque(off_t bloque);			// Indicar que un bloque fue ocupado.

void				desocuparBloque(off_t bloque);		// Indicar que un bloque fue desocupado.



// File Metadata



void				escribirArchivo(char* pathArchivo);

registroArchivo		leerArchivo(char* pathArchivo);



// Otros (Ordenados alfabéticamente)

u_int32_t			bloquesEnBytes();																		// Devuelve la cantidad de bytes que ocupa el mapa de bits.

void				cerrarDirectorioRaiz();																	// Liberar el directorio raíz.

char*				concatenarPath(char* pathIzquierdo, char* pathDerecho);									// Concatena dos directorios.

void 				crearArchivo(char* pathArchivo);														// Crea un archivo nuevo.

void				crearDirectorio(char* pathDirectorio);													// Crea un directorio y añade un archivo en él.

void				destruirBitarray(t_bitarray *bitarray);													// Libera una estructura de array de bits.

FILE*				iniciarArchivo(char* pathRelativo);														// Abre un archivo simplificado.

int					iniciarDescriptorArchivo(char* pathRelativo);											// Abre un archivo y devuelve su descriptor de archivo.

void				iniciarDirectorioRaiz(char* pathRaiz);													// Asigna el punto de montaje del FS.

void				iniciarDirectorios();																	// Crear los directorios del FS.

char*				iniciarString(size_t tamanio);															// Inicializa un char* con '\0'.

char*				obtenerDirectorio(char* pathArchivo);													// Crea los directorios con un archivo nuevo.

char*				obtenerPath(char* pathRelativo);														// Obtiene directorio de un archivo en el FS.

void				plantillaConfiguracion(char* pathConfiguracion, void(*procedimiento)(t_config*));		// Plantilla para alocar y liberar archivos de configuración.



// Definiciones



// Metadata

void iniciarMetadata(){
	plantillaMetadata(&operacionIniciarMetadata);		// Aplicación de Template method.
}

void leerMetadata(){
	plantillaMetadata(&operacionLeerMetadata);			// Aplicación de Template method.
}

void escribirMetadata(){
	plantillaMetadata(&operacionEscribirMetadata);		// Aplicación de Template method.
}

void plantillaMetadata(void(*procedimiento)(t_config*)){
	plantillaConfiguracion("/Metadata/Metadata.bin", procedimiento);
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

registroMetadata obtenerMetadata(){
	return metadata;
}



// Bitmap

void iniciarBitmap(){
	fdBitmap = iniciarDescriptorArchivo("/Metadata/Bitmap.bin");		// Abrir archivo Bitmap.bin.

	char* bitarray = iniciarString(bloquesEnBytes());					// Array de bits en 0 para indicar que todos los bloques están libres en primera instancia.

	write(fdBitmap, bitarray, bloquesEnBytes());

	free(bitarray);

	close(fdBitmap);
}

void abrirBitmap(){
	fdBitmap = iniciarDescriptorArchivo("/Metadata/Bitmap.bin");		// Abrir archivo Bitmap.bin.

	struct stat atributosBitmap;

	fstat(fdBitmap, &atributosBitmap);

	mapaDeBits = mmap(NULL, atributosBitmap.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdBitmap, 0);

	msync(mapaDeBits, bloquesEnBytes(), MS_SYNC);

	arrayDeBits = bitarray_create_with_mode(mapaDeBits, bloquesEnBytes(), LSB_FIRST);
}

void cerrarBitmap(){
	munmap(mapaDeBits, bloquesEnBytes());

	bitarray_destroy(arrayDeBits);

	close(fdBitmap);
}

t_bitarray* obtenerBitmap(){
	return arrayDeBits;
}

void ocuparBloque(off_t bloque){
	bitarray_set_bit(arrayDeBits, bloque);
}

void desocuparBloque(off_t bloque){
	bitarray_clean_bit(arrayDeBits, bloque);
}



// File Metadata





// Otros

u_int32_t bloquesEnBytes(){
	return metadata.cantidadBloques / 8;
}

void cerrarDirectorioRaiz(){
	free(puntoMontaje);
}

char* concatenarPath(char* pathIzquierdo, char* pathDerecho){
	char* path = string_new();				// Path real.

	string_append(&path, pathIzquierdo);
	string_append(&path, pathDerecho);		// Obtener path real.

	return path;
}

void crearArchivo(char* pathArchivo){
	char* path = obtenerPath(pathArchivo);

	FILE* nuevoArchivo = iniciarArchivo(pathArchivo);

	fclose(nuevoArchivo);

	free(path);
}

void crearDirectorio(char* pathDirectorio){
	char* path = obtenerPath(pathDirectorio);
	char** directorio = string_split(path, "/");

	int i = 0;
	int j = 0;

	while(directorio[i] != NULL)
	{
		mkdir(directorio[i], S_IRWXU);
		chdir(directorio[i]);

		i++;
	}

	j = i;

	while(i > 0)
	{
		chdir("..");
		i--;
	}

	while(j > 0)
	{
		free(directorio[j]);
		j--;
	}

	free(directorio);
	free(path);
}

FILE* iniciarArchivo(char* pathRelativo){
	char* path = obtenerPath(pathRelativo);

	FILE* archivo = fopen(path, "wb+");		// Abrir archivo.

	free(path);

	return archivo;
}

int iniciarDescriptorArchivo(char* pathRelativo){
	char* path = obtenerPath(pathRelativo);

	int fd = open(path, O_RDWR);		// Abrir archivo.

	free(path);

	return fd;
}

void iniciarDirectorioRaiz(char* pathRaiz){
	puntoMontaje = string_duplicate(pathRaiz);

	crearDirectorio("/");
}

void iniciarDirectorios(){
	crearDirectorio("/Metadata");
	crearArchivo("/Metadata/Metadata.bin");
	crearArchivo("/Metadata/Bitmap.bin");

	crearDirectorio("/Archivos");

	crearDirectorio("/Bloques");
}

char* iniciarString(size_t tamanio){
	char* string = malloc(tamanio);		// Array de bitmaps.
	bzero(string, tamanio);				// Setear en 0.

	return string;
}

void plantillaConfiguracion(char* pathConfiguracion, void(*procedimiento)(t_config*)){
	char* path = obtenerPath(pathConfiguracion);

	t_config* configuracion = config_create(path);

	procedimiento(configuracion);

	config_destroy(configuracion);

	free(path);
}

char* obtenerPath(char* pathRelativo){
	return concatenarPath(puntoMontaje, pathRelativo);
}

#endif /* FILESYSTEM_SADICA_H_ */
