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

typedef struct {
	int pid;
	int nroPagina;
} strCache;

int* ultimoAcceso;

int ENTRADAS_CACHE = 10;
int CACHE_X_PROC;

void escribirAdmCache();
void iniciarAdmCache(int* cache);
int compararDatos(strCache a, strCache b);
int entradaMasAntigua();

void iniciarDatosConfiguracionCache() {
	t_config* configuracion = asignarRutaDeArchivo();
	ENTRADAS_CACHE = busquedaClaveNumerica(configuracion, "ENTRADAS_CACHE");
	CACHE_X_PROC = busquedaClaveNumerica(configuracion, "CACHE_X_PROC");
}

int* reservarCache() {
	int sizeCache = sizeof(strCache);
	iniciarDatosConfiguracionCache();
	int* ptrCache = malloc(ENTRADAS_CACHE * (sizeCache + getFrameSize()));
	//ENTRADAS CACHE * 4
	ultimoAcceso = malloc(ENTRADAS_CACHE * sizeof(temporal_get_string_time()));
	return ptrCache;
}

//Setea todos los datos de la str adm de la cache en -1
void iniciarAdmCache(int* cache) {

	int movimiento = sizeof(strCache) + getFrameSize();
	int entrada = 0;
	int movimientoChar = sizeof(temporal_get_string_time());
	strCache aux;

	while (entrada < ENTRADAS_CACHE) {
		aux.nroPagina = -1;
		aux.pid = -1;
		//setea en -1 el pid y el nro de pagina de la str adm
		memcpy(cache + entrada * movimiento, &aux, sizeof(strCache));
		//Setea en la hora actual el ultimo acceso, en la estructura paralela
		memcpy(ultimoAcceso + entrada * movimientoChar,
				temporal_get_string_time(), movimientoChar);
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
	info.pid=pid;
	info.nroPagina=pag;
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

//Copia el contenido de una entrada de cache y lo devulve
void leerEntrada(int* cache, int posicion, void* contenido) {

	int sizeStr = sizeof(strCache);
	int movimiento = sizeStr + getFrameSize();

	memcpy(contenido, cache + posicion * movimiento + sizeStr, getFrameSize());
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
	while (c < ENTRADAS_CACHE) {
		memcpy(&auxB, cache + c * movimiento, sizeStr);
		if (compararDatos(aux, auxB))
			return c;
		c++;
	}
	return -1;
}

void ingresarNuevaEntrada(int* ptrCache, int pid, int pagina,void* aux) {
	int entradaVieja=entradaMasAntigua();
	//Escribo la cache en la posicion, en la que la ultima que entrada tiene el menor acceso
	escribirCache(ptrCache,pid,pagina,aux,entradaVieja);
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
	int c = 0, posicion=0;
	int comparacion;

	while (c < ENTRADAS_CACHE) {
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
