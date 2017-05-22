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
//#include "../librerias/funcionesMemoria.h"

#define RUTA_ARCHIVO "./config_memoria.cfg"
#define LIBRE -7
#define OCUPADO_x_STR -14
#define NOT_FOUND -28

//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION
int PUERTO_MEMORIA;
int FRAME_SIZE;
int FRAMES;
int ENTRADAS_CACHE;
int CACHE_X_PROC;
int RETARDO_MEMORIA;

t_log *archivoLog;

typedef struct {
	int pid;
	int nroPagina;
	int frame;
}strAdministrativa;


void asignarDatosDeConfiguracion(){
	t_config* configuracion;
	char* ruta = RUTA_ARCHIVO;
	configuracion = llamarArchivo(ruta);

	PUERTO_MEMORIA = busquedaClaveNumerica(configuracion, "PUERTO");
	FRAME_SIZE = busquedaClaveNumerica(configuracion, "MARCO_SIZE");
	FRAMES = busquedaClaveNumerica(configuracion, "MARCOS");
	ENTRADAS_CACHE = busquedaClaveNumerica(configuracion, "ENTRADAS_CACHE");
	CACHE_X_PROC = busquedaClaveNumerica(configuracion, "CACHE_X_PROC");
	RETARDO_MEMORIA = busquedaClaveNumerica(configuracion, "RETARDO_MEMORIA");
}

//Reservo la memoria que necesito
int* reservarMemoria(){
	int* memoria;
	memoria=malloc(FRAME_SIZE * FRAMES);
	if (memoria == NULL)
		esErrorConSalida(-1,"Error en la solicitud de bloque de memoria (Malloc)");
	return memoria;
}

//Seteo todas las estructuras con el valor predefinido PID=-14 de esta manera puedo saber que el frame no tiene un proceso asignado
void inicializarStrAdm(int* bloqueMemoria){

	int sizeStrAdm;
	int sizeBlockAdm;
	int contador_frame = 0; // contador para los frames dentro del for
	strAdministrativa auxiliar;


	//El tamaño de la estructura por la cantidad de FRAMES 500*256B = 6000 B
	//Si hacemos 6000/MARCO_SIZE nos da 23,43, por lo cual le sumamos un marco para que asi nos de 24.
	//De esta manera tenemos 24 FRAMES ocupados por la memoria administrativa
	sizeStrAdm = sizeof(strAdministrativa);
	sizeBlockAdm = ((sizeStrAdm*FRAMES)+FRAME_SIZE)/FRAMES;

	auxiliar.pid=LIBRE;
	auxiliar.frame=contador_frame;
	auxiliar.nroPagina=LIBRE;

	//mientras la cantidad de frames del contador sea menor a 500 (FRAMES)
	while(contador_frame<FRAMES){
		//Con este if nos aseguramos que hay 12 frames utilizados para las estructuras administrativas
		//de esta manera cuando guardemos info no vamos a pisar nada
		if(contador_frame>=FRAMES-sizeBlockAdm){
			auxiliar.pid=OCUPADO_x_STR;
			auxiliar.nroPagina=contador_frame;
		}
		else{
		auxiliar.pid=LIBRE;
		auxiliar.nroPagina=LIBRE;
		}
		//Esta funcion copia los datos The memcpy(DEST,SRC,BYTES) function copies n bytes from memory area src to memory area dest.
		memcpy(bloqueMemoria+contador_frame*sizeStrAdm,&auxiliar,sizeStrAdm);
		contador_frame++;
		auxiliar.frame=contador_frame;
	}

	log_info(archivoLog,"El bloque de memorias administrativas ocupa %d",sizeBlockAdm);
	log_info(archivoLog,"Son %d estructuras con un peso individual de %d",FRAMES,sizeStrAdm);
}


t_log* crearArchivo(char* ruta){

	archivoLog = log_create(ruta,"Memoria",true,LOG_LEVEL_INFO);
	return archivoLog;
}

void iniciarStrAdmProg(int pid, int frames, int* memoria){

	strAdministrativa aux;



}

void imprimirStrAdm(int* bloqueMemoria){
	strAdministrativa aux;
	int c=0;
	while(c<FRAMES){
		memcpy(&aux,bloqueMemoria+c*sizeof(strAdministrativa),sizeof(strAdministrativa));
		printf("\nPID: %d, NroPagina: %d, Frame: %d",aux.pid,aux.nroPagina,aux.frame);
		//log_info(archivoLog,"\n*******************************************\n");
		//log_info(archivoLog,"PID: %d, NroPagina: %d, Frame: %d",aux.pid,aux.nroPagina,aux.frame);
		c++;
	}
}

//La estructura se encuentra libre siempre y cuando su PID sea igual a -7
int strLibre(int* ptr){
	strAdministrativa aux;
	memcpy(&aux,ptr,sizeof(strAdministrativa));
	if(aux.pid==LIBRE)
		return 1;
	else
		return 0;

}




void liberarStrAdm(int pid,int* ptr){
	int c=-1;
	strAdministrativa aux;
	do{
		c++;
		memcpy(&aux,ptr+c*sizeof(strAdministrativa),sizeof(strAdministrativa));
	}while(c<FRAMES&&aux.pid!=pid);

	aux.pid=LIBRE;
	aux.nroPagina=LIBRE;
	aux.frame=c;
	memcpy(ptr+c*sizeof(strAdministrativa),&aux,sizeof(strAdministrativa));

}
int getPuertoMemoria(){
	return PUERTO_MEMORIA;
}
t_log* getArchivoLog(){
	return archivoLog;
}

int buscarPID(int pid, int* memoria){
	strAdministrativa aux;
	int countFrame=-1;
	do{
		countFrame++;
		memcpy(&aux,memoria+countFrame*sizeof(strAdministrativa),sizeof(strAdministrativa));
		if(aux.pid==pid)
			return aux.frame;
	}while(countFrame<FRAMES);

	return NOT_FOUND;
}

int buscarStrAdm(int pid, int* memoria){
	strAdministrativa aux;
		int countFrame=-1;
		do{
			countFrame++;
			memcpy(&aux,memoria+countFrame*sizeof(strAdministrativa),sizeof(strAdministrativa));
			if(aux.pid==pid)
				return countFrame;
		}while(countFrame<FRAMES);

	return NOT_FOUND;

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

void recibir (){}

void enviar (int* socket, char* buffer ){}

#endif /* FUNCIONESMEMORIA_H_ */
