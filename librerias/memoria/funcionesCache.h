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

void iniciarDatosConfiguracionCache() {
	t_config* configuracion = asignarRutaDeArchivo();
	ENTRADAS_CACHE = busquedaClaveNumerica(configuracion, "ENTRADAS_CACHE");
	CACHE_X_PROC = busquedaClaveNumerica(configuracion, "CACHE_X_PROC");
}

int* reservarCache() {
	int sizeCache = sizeof(strCache);
	iniciarDatosConfiguracionCache();
	int* ptrCache = malloc(ENTRADAS_CACHE * (sizeCache + getFrameSize()));
	ultimoAcceso = malloc(ENTRADAS_CACHE * sizeof(temporal_get_string_time()));
	return ptrCache;
}

void iniciarAdmCache(int* cache) {

	int movimiento = sizeof(strCache) + getFrameSize();
	int entrada = 0;
	int movimientoChar = sizeof(temporal_get_string_time());
	strCache aux;

	while (entrada < ENTRADAS_CACHE) {
		aux.nroPagina = -1;
		aux.pid = -1;

		memcpy(cache + entrada * movimiento, &aux, sizeof(strCache));
		memcpy(ultimoAcceso + entrada * movimientoChar,
				temporal_get_string_time(), movimientoChar);
		entrada++;
	}

}

void escribirAdmCache(int posicion) {
	int movimiento = sizeof(temporal_get_string_time());
	memcpy(ultimoAcceso + posicion * movimiento, temporal_get_string_time(),
			movimiento);
}
void escribirCache(int* cache, strCache entrada, void* contenido,
		int posicion) {

	int movimiento = getFrameSize() + sizeof(strCache);

	memcpy(cache + posicion * movimiento, &entrada, sizeof(strCache));
	memcpy(cache + posicion * movimiento + sizeof(strCache), &contenido,
			getFrameSize());

	escribirAdmCache(posicion);
}
char* leerAdmCache(int posicion) {
	int movimiento = sizeof(temporal_get_string_time());
	char* valor;
	memcpy(&valor, ultimoAcceso + posicion * movimiento, movimiento);
	return valor;
}

void* leerEntrada(int* cache, int posicion) {
	void* contenido;
	int sizeStr = sizeof(strCache);
	int movimiento = sizeStr + getFrameSize();

	memcpy(&contenido, cache + posicion * movimiento + sizeStr, getFrameSize());

	return contenido;
}

int buscarEntrada(int* cache, int pid, int pagina) {
	strCache aux, auxB;
	int sizeStr = sizeof(strCache);
	int movimiento = sizeStr + getFrameSize();
	int c = 0;
	aux.pid = pid;
	aux.nroPagina = pagina;
	while (c < ENTRADAS_CACHE) {
		memcpy(&auxB,cache+c*movimiento,sizeStr);
		if(compararDatos(aux,auxB))
			return c;
		c++;
	}
	return -1;
}

int compararDatos(strCache a, strCache b) {
	if (a.pid == b.pid && a.nroPagina == b.nroPagina)
		return 1;
	else
		return 0;
}

char* entradaMasAntigua(int posA, int posB) {
	int comparacion = strcmp(leerAdmCache(posA), leerAdmCache(posB));
	//Si es menor a cero posA es menor que posB, por el contrario si es amyor a cero, entonces posA es mayor a posB
	if (comparacion < 0)
		return posA;
	else
		return posB;

}

#endif /* MEMORIA_FUNCIONESCACHE_H_ */
