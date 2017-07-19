/*
 * funcionesCache.h
 *
 *  Created on: 19/6/2017
 *      Author: utnso
 */

#ifndef MEMORIA_FUNCIONESCACHE_H_
#define MEMORIA_FUNCIONESCACHE_H_

#include <netinet/in.h>
#include <commons/log.h>
#include <commons/temporal.h>
#include <string.h>

#include "../controlArchivosDeConfiguracion.h"
#include "../serializador.h"
#include "./funcionesMemoria.h"
//#include "../../memoria/memoria.c"

#define LIBRE -1

typedef struct {
	int pid;
	int nroPagina;
} strCache;

int* ultimoAcceso;

int _entradasCache = 10;
int _cacheXPrc;

void iniciarDatosConfiguracionCache();
int* reservarCache() ;
void iniciarAdmCache(int* cache);
void escribirUltimoAcceso(int posicion) ;
void escribirCache(int* cache, int pid, int pag, void* contenido, int posicion);
char* leerUltimoAcceso(int posicion);
void imprimirEntradasActivas();
void imprimirEntradasSinContenido();
void leerEntrada(int* cache, int posicion, strCache *adm, void* contenido) ;
void leerAdmCache(int* cache, int posicion, strCache *adm);
void leerContenido(int* cache, int posicion, void* contenido);
int buscarEntrada(int* cache, int pid, int pagina);
void ingresarNuevaEntrada(int* ptrCache, int pid, int pagina, void* aux) ;
int compararDatos(strCache a, strCache b);
int entradaMasAntigua() ;
int getStrCache();


void iniciarDatosConfiguracionCache() {
	t_config* configuracion = asignarRutaDeArchivo();
	_entradasCache = busquedaClaveNumerica(configuracion, "ENTRADAS_CACHE");
	_cacheXPrc = busquedaClaveNumerica(configuracion, "CACHE_X_PROC");
}

int* reservarCache() {
	int sizeCache = sizeof(strCache);
	iniciarDatosConfiguracionCache();
	int* ptrCache = malloc(_entradasCache * (sizeCache + getFrameSize()));
	//ENTRADAS CACHE * 4
	ultimoAcceso = malloc(_entradasCache * sizeof(temporal_get_string_time()));
	return ptrCache;
}

//Setea todos los datos de la str adm de la cache en -1
void iniciarAdmCache(int* cache) {

	int movimiento = sizeof(strCache) + _frameSize;
	int entrada = 0;
	strCache aux;
	aux.nroPagina = LIBRE;
	aux.pid = LIBRE;
	while (entrada < _entradasCache) {
		//setea en -1 el pid y el nro de pagina de la str adm
		memcpy(cache + entrada * movimiento, &aux, sizeof(strCache));
		//Setea en la hora actual el ultimo acceso, en la estructura paralela
		escribirUltimoAcceso(entrada);
		entrada++;
	}

}

// Escribe la hora actual en de una determinada entrada en la cache
void escribirUltimoAcceso(int posicion) {
	int movimiento = sizeof(temporal_get_string_time());
	memcpy(ultimoAcceso + posicion * movimiento, temporal_get_string_time(),
			movimiento);
}

//Escribe en la cache
void escribirCache(int* cache, int pid, int pag, void* contenido,
		int posicion) {
	strCache info;
	info.pid = pid;
	info.nroPagina = pag;
	int movimiento = getFrameSize() + sizeof(strCache);

	//Escribe la entrada de la str Cache
	memcpy(cache + posicion * movimiento, &info, sizeof(strCache));
	//Escribe el contenido de la pagina sacada de memoria
	memcpy(cache + posicion * movimiento + sizeof(strCache), &contenido,
			getFrameSize());

	escribirUltimoAcceso(posicion);
}
char* leerUltimoAcceso(int posicion) {
	int movimiento = sizeof(temporal_get_string_time());
	char* valor;
	memcpy(&valor, ultimoAcceso + posicion * movimiento, movimiento);
	return valor;
}

void imprimirEntradasActivas() {
	void *contenido = malloc(_frameSize);
	strCache adm;
	int c;
	log_info(archivoLog, "*** INICIO IMPRESION DE ENTRADAS CACHE ACTIVAS ***");
	while (c < _entradasCache) {

		leerEntrada(ptrCache, c, &adm, contenido);
		if (adm.pid != LIBRE) {
			log_info(archivoLog, "PID: %d", adm.pid);
			log_info(archivoLog, "Pag: %d", adm.nroPagina);
			log_info(archivoLog, "Contenido:\n", (char *) contenido);
		}
		c++;
	}
	log_info(archivoLog, "*** FIN ***");
	free(contenido);
}

void imprimirEntradasSinContenido() {

	void *contenido = malloc(_frameSize);
	strCache adm;
	int c=0;
	log_info(archivoLog, "*** INICIO IMPRESION DE ENTRADAS CACHE ***");
	while (c < _entradasCache) {
		leerAdmCache(ptrCache, c, &adm);
		log_info(archivoLog, "PID: %d", adm.pid);
		log_info(archivoLog, "Pag: %d", adm.nroPagina);
		c++;
	}
	log_info(archivoLog, "*** FIN ***");
	free(contenido);
}

//Copia el contenido de una entrada de cache y lo devulve
void leerEntrada(int* cache, int posicion, strCache *adm, void* contenido) {

	int sizeStr = sizeof(strCache);
	int movimiento = sizeStr + getFrameSize();
	memcpy(adm, cache + posicion * movimiento, sizeStr);
	memcpy(contenido, cache + posicion * movimiento + sizeStr, getFrameSize());
	escribirUltimoAcceso(posicion);
}
void leerAdmCache(int* cache, int posicion, strCache *adm) {
	int sizeStr = sizeof(strCache);
	int movimiento = sizeStr + _frameSize;
	memcpy(adm, cache + posicion * movimiento, sizeStr);
}
void leerContenido(int* cache, int posicion, void* contenido) {
	int sizeStr = sizeof(strCache);
	int movimiento = 2 * sizeStr + _frameSize;
	memcpy(contenido, cache + posicion * movimiento, _frameSize);
	escribirUltimoAcceso(posicion);
}

//Busca una entrada dentro de la cache a partir de los datos pasados, si la encuentra
//devuelve la posicion en la cache
int buscarEntrada(int* cache, int pid, int pagina) {
	strCache aux, auxB;
	int sizeStr = sizeof(strCache);
	int movimiento = sizeStr + getFrameSize();
	int c = 0;
	aux.pid = pid;
	aux.nroPagina = pagina;
	while (c < _entradasCache) {
		memcpy(&auxB, cache + c * movimiento, sizeStr);
		if (compararDatos(aux, auxB))
			return c;
		c++;
	}
	return -1;
}

void ingresarNuevaEntrada(int* ptrCache, int pid, int pagina, void* aux) {
	int entradaVieja = entradaMasAntigua();
	//Escribo la cache en la posicion, en la que la ultima que entrada tiene el menor acceso
	escribirCache(ptrCache, pid, pagina, aux, entradaVieja);
}

//----------------- COMPARAR DATOS -----------------

//Compara dos estructuras administrativas de cache
int compararDatos(strCache a, strCache b) {
	if (a.pid == b.pid && a.nroPagina == b.nroPagina)
		return 1;
	else
		return 0;
}

// Recibe dos posiciones y devuelve la posicion que tenga la entrada mas vieja
int entradaMasAntigua() {
	int c = 0, posicion = 0;
	int comparacion;

	while (c < _entradasCache) {
		//Si es menor a cero posA es menor que posB, por el contrario si es amyor a cero, entonces posA es mayor a posB
		comparacion = strcmp(leerUltimoAcceso(posicion), leerUltimoAcceso(c));
		if (comparacion > 0)
			posicion = c;
		/*

		 Nota: 	Si el 1er parametro es menor al 2do => compatacion < 0
		 Si el 1er parametro es igual al 2do => comparacion = 0
		 si el 1er parametro es mayor al 2do => comparacion > 0
		 */
		c++;
	}
	return posicion;
}


int getStrCache() {
	return sizeof(strCache);
}

#endif /* MEMORIA_FUNCIONESCACHE_H_ */
