/*
 * funcionesMemoria.h
 *
 *  Created on: 24/04/2017
 *      Author: Zero_Gravity
 */

#ifndef FUNCIONESMEMORIA_H_
#define FUNCIONESMEMORIA_H_

#include <netinet/in.h>
#include <commons/log.h>
#include <string.h>

#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/funcionesMemoria.h"
#include "../librerias/operacionesMemoria.h"

#define RUTA_ARCHIVO "./config_memoria.cfg"
#define PROCESO_VACIO -14
#define PAG_LIBRE -7

t_log *archivoLog;

typedef struct {
	int pid;
	int nroPagina;
	int frame;
}strAdministrativa;

void asignarDatosDeConfiguracion(int* PUERTO_MEMORIA, int* MARCO_SIZE, int* MARCOS, int* ENTRADAS_CACHE, int* CACHE_X_PROC, int* RETARDO_MEMORIA){
	t_config* configuracion;
	char* ruta = RUTA_ARCHIVO;
	configuracion = llamarArchivo(ruta);

	*PUERTO_MEMORIA = busquedaClaveNumerica(configuracion, "PUERTO");
	*MARCO_SIZE = busquedaClaveNumerica(configuracion, "MARCO_SIZE");
	*MARCOS = busquedaClaveNumerica(configuracion, "MARCOS");
	*ENTRADAS_CACHE = busquedaClaveNumerica(configuracion, "ENTRADAS_CACHE");
	*CACHE_X_PROC = busquedaClaveNumerica(configuracion, "CACHE_X_PROC");
	*RETARDO_MEMORIA = busquedaClaveNumerica(configuracion, "RETARDO_MEMORIA");
}

//Seteo todas las estructuras con el valor predefinido PID=-14 de esta manera puedo saber que el frame no tiene un proceso asignado
void inicializarStrAdm(int FRAMES, int FRAME_SIZE, int* bloqueMemoria){

	//El tamaño de la estructura por la cantidad de FRAMES 500*256B = 6000 B
	//Si hacemos 6000/MARCO_SIZE nos da 23,43, por lo cual le sumamos un marco para que asi nos de 24.
	//De esta manera tenemos 24 FRAMES ocupados por la memoria administrativa
	int sizeStrAdm;
	int sizeBlockAdm;
	int contador_frame = 0; // contador para los frames dentro del for
	strAdministrativa auxiliar;

	sizeStrAdm = sizeof(strAdministrativa);
	sizeBlockAdm = ((sizeStrAdm*FRAMES)+FRAME_SIZE-1)/FRAMES;

	auxiliar.pid=PROCESO_VACIO;
	auxiliar.frame=contador_frame;
	auxiliar.nroPagina=PAG_LIBRE;

	while(contador_frame<FRAMES){

		//inicializo los valores del frame del bloque administrativo de esta manera despues podemos distinguir un frame vacio de uno ocupado
		if(contador_frame<sizeBlockAdm){
			auxiliar.pid=PROCESO_VACIO;
			auxiliar.nroPagina=contador_frame;
		}
		else{
			auxiliar.pid=PAG_LIBRE;
			auxiliar.nroPagina=PAG_LIBRE;
		}



		//Esta funcion copia los datos The memcpy() function copies n bytes from memory area src to memory area dest.
		memcpy(bloqueMemoria+contador_frame*sizeStrAdm,&auxiliar,sizeStrAdm);
		contador_frame++;
		auxiliar.frame=contador_frame;
	}

	log_info(archivoLog,"El bloque de memorias administrativas ocupa %d",sizeBlockAdm);
	log_info(archivoLog,"Son %d estructuras con un peso individual de %d",FRAMES,sizeStrAdm);
}

void crearArchivo(char* ruta){

	archivoLog = log_create(ruta,"Memoria",true,LOG_LEVEL_INFO);
}












//Modificar la cantidad de tiempo que espera el proceso memoria antes de responder una solicitud
void retardo (int tiempoEspera){}

//Genera un reporte en pantalla y en un archivo en disco del estado actual del 
//cache, estructuras de memoria, contenido de memoria
void dump (){}

//Este comando debera limpiar el contenido de la cache
void flush (int *ptrCache){}

//MEMORY: Indicara el tamaño de la memoria en cant. de frames, la cant. de frames ocupados y los libres
//PID: Indicara el tamaño total de un procoeso
void size (){}



#endif /* FUNCIONESMEMORIA_H_ */
