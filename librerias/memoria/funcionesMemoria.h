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

#include "../controlArchivosDeConfiguracion.h"
//#include "funcionesCache.h"
#include "../serializador.h"


#define RUTA_ARCHIVO "config_memoria.cfg"
#define LIBRE -1
#define OCUPADO_x_STR -14
#define NOT_FOUND -28

//ESTRUCTURAS

typedef struct {
	int pid;
	int nroPagina;
	int frame;
} strAdministrativa;

//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION
int _puertoMemoria;
int _frameSize;
int _frames;
int _retardoMemoria;

extern int* ptrMemoria;
extern int* ptrCache;
//ARCHIVOS LOG
t_log *archivoLog,*archivoSP;




//  ******* DECLARACION DE FUNCIONES  *******
//estoy podrida de los warnings

int getPuertoMemoria();
int getFrameSize();
int getSizeStrAdm();
int getRetardo();

void enviarMensaje(int socket, void* buffer, size_t tamanioBuffer);
int recibirSeleccionOperacion(int socket);

void consolaMemoria();
void size();
void flush() ;
void dump() ;
void contenidoDeMemoria();
void verContenidoDeProceso(int pid);
void leerContenidoPosicion(int frame,void* contenido);
void verContenidoDeMemoria();
void modificarRetardo(int tiempoEspera);
void menuDump();
void menuConsola();
void imprimirStrAdm(int* bloqueMemoria);
int buscaStrPID(int pid, strAdministrativa *adm);
int buscarFrameDePagina(int pid, int *ptr, int pagina);
int buscarFrameEnMemoria(int pid, int *ptr);
int strVacia(int *ptr);
int paginasOcupadasEnMemoria ();
int paginasLibresEnMemoria(int *ptr);
int paginasPrc(int pid, int*ptr);
int compararPID(strAdministrativa aux,int pid);
int asignacionDePaginas(int cantPaginas,int pid,int *ptr);
int iniciarStrAdmDePrc(int pid, int c, int* ptr);
void liberarStrAdm(int pid, int* ptr) ;
int strLibre(int* ptr) ;
void inicializarStrAdm(int* bloqueMemoria);

void recibirDatosPID_PAG(int socket, int *pid, int *pag);
void recibirDatosTotal(int socket, int* pid, int *pag, int* offset, int *size);



t_log* getArchivoSP();
t_log* getArchivoLog();
void crearArchivo(char* ruta) ;

int* reservarMemoria();
void asignarDatosDeConfiguracion();
t_config* asignarRutaDeArchivo();

// ******* INICIO DE CODIGO  *******

t_config* asignarRutaDeArchivo(){
	t_config* llamadaArchivo;
	char* ruta = RUTA_ARCHIVO;
	llamadaArchivo= llamarArchivo(ruta);
	return llamadaArchivo;
}

void asignarDatosDeConfiguracion() {
	t_config* configuracion = asignarRutaDeArchivo();

	char* puerto = "PUERTO";
	_puertoMemoria = busquedaClaveNumerica(configuracion, puerto);
	_frameSize = busquedaClaveNumerica(configuracion, "MARCO_SIZE");
	_frames = busquedaClaveNumerica(configuracion, "MARCOS");
	_retardoMemoria = busquedaClaveNumerica(configuracion, "RETARDO_MEMORIA");
}

//  ******* FUNCIONES DE BLOQUE DE MEMORIA *******

//Reservo la memoria que necesito
int* reservarMemoria() {
	int* memoria;
	memoria = malloc(_frameSize * _frames);
	if (memoria == NULL)
		esErrorConSalida(-1,
				"Error en la solicitud de bloque de memoria (Malloc)");
	return memoria;
}

//  ******* FUNCIONES DE ARCHIVO LOG  *******

void crearArchivo(char* ruta) {

	mkdir("../archivosLog",0755);
	archivoLog= log_create(ruta, "Memoria", true, LOG_LEVEL_INFO);
	archivoSP= log_create(ruta, "Memoria", false, LOG_LEVEL_INFO);
}

t_log* getArchivoLog(){
	return archivoLog;
}

t_log* getArchivoSP(){
	return archivoSP;
}

//  ******* FUNCIONES DE CONTROL DE STR ADMINISTRATIVA  *******

//Seteo todas las estructuras con el valor predefinido PID=-14 de esta manera puedo saber que el frame no tiene un proceso asignado
void inicializarStrAdm(int* bloqueMemoria) {

	int paginaOcupada=0;
	int sizeStrAdm;
	int sizeBlockAdm;
	int contadorFrame = 0; // contador para los frames dentro del for
	strAdministrativa auxiliar;

	//El tamaño de la estructura por la cantidad de _frames 500*256B = 6000 B
	//Si hacemos 6000/_frames nos da 12, por lo cual le sumamos dos marcos 6.512
	// y le restamos 12 para que al dividirlo nos de 13 marcos
	sizeStrAdm = sizeof(strAdministrativa);
	sizeBlockAdm = ((sizeStrAdm * _frames) + _frameSize) / _frameSize;

	auxiliar.pid = LIBRE;
	auxiliar.frame = contadorFrame;
	auxiliar.nroPagina = LIBRE;

	//mientras la cantidad de frames del contador sea menor a 500 (_frames)
	while (contadorFrame < _frames) {

		//Con este if nos aseguramos que hay 12 frames utilizados para las estructuras administrativas
		//de esta manera cuando guardemos info no vamos a pisar nada
		if (contadorFrame < sizeBlockAdm) {
			auxiliar.pid = OCUPADO_x_STR;
			auxiliar.nroPagina = paginaOcupada;
			paginaOcupada++;
		} else {
			auxiliar.pid = LIBRE;
			auxiliar.nroPagina = LIBRE;
		}
		//Esta funcion copia los datos The memcpy(DEST,SRC,BYTES) function copies n bytes from memory area src to memory area dest.
		memcpy(bloqueMemoria + contadorFrame * sizeStrAdm, &auxiliar,
				sizeStrAdm);
		contadorFrame++;
		auxiliar.frame = contadorFrame;
	}

	log_info(archivoLog, "El bloque de memorias administrativas ocupa %d frames",sizeBlockAdm);
	log_info(archivoLog, "Son %d estructuras con un peso individual de %d Bytes",_frames, sizeStrAdm);

}

//La estructura se encuentra libre siempre y cuando su PID sea igual a -7
int strLibre(int* ptr) {
	strAdministrativa aux;
	memcpy(&aux, ptr, sizeof(strAdministrativa));
	if (aux.pid == LIBRE)
		return 1;
	else
		return 0;

}

//Al liberar una estructura administrativa se le asigna al pid y al nro pagina el nro -7
void liberarStrAdm(int pid, int* ptr) {
	int c=0;
	strAdministrativa aux;

	//Mientras contador sea menor a la cant de frames,
	//el ptero se sigue moviendo de a una estructura adm
	while(c<_frames){

		//Copio los datos apuntos en la direccion de memoria a un auxiliar de estructura
		memcpy(&aux, ptr + c * sizeof(strAdministrativa),sizeof(strAdministrativa));

		//Si el pid de la estructura apuntada es el indicado, seteo los datos de aux
		//para que su pid y nro de pagina sean -7 indicador de la str libre
		//el nro de frame lo dejo intacto por que no es necesario modificarlo

		if(compararPID(aux,pid)){
		aux.pid = LIBRE;
		aux.nroPagina = LIBRE;

		//copio los nuevos datos a la direccion de memoria
		memcpy(ptr + c * sizeof(strAdministrativa), &aux,sizeof(strAdministrativa));
		}

		// sumo uno para encontrar todos las str con el pid necesario
		c++;
	}



}


//Inicia la str adm de un proceso, con un pid indicado en un determinado frame
int iniciarStrAdmDePrc(int pid, int c, int* ptr) {

	strAdministrativa aux;
	aux.pid=pid;

	//Se fija la cantidad de paginas que tiene el proceso en toda la EA (estructura Adm)
	aux.nroPagina = paginasPrc(pid, ptr);

	aux.frame = c ;
	memcpy(ptr,&aux,sizeof(strAdministrativa));

 return 1;
}


//  ******* FUNCIONES DE ASIGNACION  *******


//Esta funcion asigna las paginas una por una, busca la primera estructura vacia y le asigna los datos, asi sucesivamente hasta que llega todas las paginas requeridas
int asignacionDePaginas(int cantPaginas,int pid,int *ptr){

	int c=0,paginasAsignadas=0,iniciarPrograma;

	//1ro. Mientras las pag Asignadas sean menos que las pag que necesito se repite el ciclo
	while(paginasAsignadas<cantPaginas){

		//2do. Si el frame al que apunto esta vacio entonces asigno los datos
		if(strVacia(ptr+c*sizeof(strAdministrativa))){
			iniciarPrograma=0;

			//Se le envia a iniciar... el nro de pid, el nro de frame con el que trabajara, y el puntero al frame vacio de la strAdm,
			//devuelve un 1 en caso de que la operacion haya podido realizarse y un 0 en caso contrario
			iniciarPrograma=iniciarStrAdmDePrc(pid,c,ptr+c*sizeof(strAdministrativa));

			if(iniciarPrograma)
				paginasAsignadas++;

		}//Cierro If Frame Vacio

		//3ro. Me muevo hacia la siguiente str Adm. y vuelvo a hacer los controles 1 y 2
		c++;
	}
	return paginasAsignadas;
}


//  ******* FUNCIONES DE BUSQUEDA  *******

//Compara dos pid, el de una EA aux y el pid que le pasa
int compararPID(strAdministrativa aux,int pid){

	if(aux.pid==pid)
		return 1;
	else
		return 0;

}

//Cuenta la cantidad de paginas que tiene asignado un proceso en memoria
int paginasPrc(int pid, int*ptr){
	int paginasPrc=0,c=0;
	strAdministrativa aux;
	size_t size=sizeof(strAdministrativa);
	while(c<_frames){

		memcpy(&aux,ptr+c*sizeof(strAdministrativa),size);
		if(compararPID(aux,pid)){
			paginasPrc++;
		}
		c++;

	}

	return paginasPrc;
}

//Devuelve la cantidad de paginas libres que hay en toda la memoria
int paginasLibresEnMemoria(int *ptr){

		int c=0, paginasLibres=0;
		size_t size=sizeof(strAdministrativa);
		strAdministrativa aux;
		while(c<_frames){
			memcpy(&aux, ptr+c*size,size);
			if(compararPID(aux,LIBRE)){
				paginasLibres++;
			}
			c++;
		}

		return paginasLibres;

}
int paginasOcupadasEnMemoria (){

	return _frames-paginasLibresEnMemoria(ptrMemoria);
}

//Controla si una estructura administrativa esta vacia, no tiene asignada ningun proceso
int strVacia(int *ptr){

	//C hace referencia a la posicion en la que se encuentra el frame vacio
	int c=0;
	int frameLibre=NOT_FOUND;
	strAdministrativa aux;
	size_t size=sizeof(strAdministrativa);
	while(c<_frames){
		memcpy(&aux, ptr+c*sizeof(strAdministrativa),size);
		if(compararPID(aux,LIBRE)){
			frameLibre=c;
		}
		c++;
	}
	return frameLibre;
}

//Buscar el frame correspondiente a un pid
int buscarFrameEnMemoria(int pid, int *ptr){
	int c=0;
	strAdministrativa aux;
	while(c<_frames){
		memcpy(&aux,ptr+c*getSizeStrAdm(),getSizeStrAdm());
		if(compararPID(aux, pid))
			return aux.frame;
		c++;
	}

	return NOT_FOUND;
}

int buscarFrameDePagina(int pid, int *ptr, int pagina){
	int contador;
	strAdministrativa aux;

	while (contador<_frames){

		memcpy(&aux, ptr+contador*sizeof(strAdministrativa),sizeof(strAdministrativa));
		if(compararPID(aux,pid)&&aux.nroPagina==pagina){
			return aux.frame;
		}
		contador++;
	}
	return NOT_FOUND;
}

int buscaStrPID(int pid, strAdministrativa *adm){
	int c=0;
	while(c<_frames){
		memcpy(adm,ptrMemoria+c*getSizeStrAdm(),getSizeStrAdm());
		if(compararPID(*adm, pid))
			return 1;
		c++;
	}

	return NOT_FOUND;

}

//  ******* FUNCIONES DE IMPRESION DE MEMORIA  *******

//Como su nombre lo indica imprime todas las estructuras adm en pantalla
void imprimirStrAdm(int* bloqueMemoria) {
	strAdministrativa aux;
	int c = 0;
	log_info(archivoLog,"***INICIO DE IMPRESION DE LA TABLA DE PAGINAS***");
	while (c < _frames) {
		memcpy(&aux, bloqueMemoria + c * sizeof(strAdministrativa),	sizeof(strAdministrativa));
		//printf("\nPID: %d, NroPagina: %d, Frame: %d", aux.pid, aux.nroPagina,	aux.frame);
		log_info(archivoLog,"*** PID = %d /// PAG = %d /// FRAME = %d ***",aux.pid, aux.nroPagina,aux.frame);
		c++;
	}
	log_info(archivoLog,"*** FIN ***");
}




//  ******* FUNCIONES DE CONSOLA DE LA MEMORIA  *******
void menuConsola(){

		printf("\n1.Modificar modificarRetardo de operacion en la memoria");
		printf("\n2.Reporte de contenido");
		printf("\n3.Limpiar la memoria cache - FLUSH");
		printf("\n4.Indicar tamano");
		printf("\n5.Salir de la consola");
}

void menuDump(){
	printf("Reportes completos de: ");
	printf("\n1.La memoria cache");
	printf("\n2.La tabla de paginas y los procesos activos");
	printf("\n3.Datos almacenados en todos los procesos o en uno en particular");
}


//Modificar la cantidad de tiempo que espera el proceso memoria antes de responder una solicitud
void modificarRetardo(int tiempoEspera) {
	_retardoMemoria = tiempoEspera;
	printf("El nuevo retardo es de %d segundo",_retardoMemoria);
}

void verContenidoDeMemoria(){
	int c=0;
	strAdministrativa adm;
	void* contenido = malloc(_frameSize);
	log_info(archivoLog,"*** INICIO IMPRESION CONTENIDO MEMORIA ***");
	while(c<_frames){
		memcpy(&adm,ptrMemoria+c*sizeof(strAdministrativa),sizeof(strAdministrativa));
		if(adm.pid!=LIBRE&&adm.pid!=OCUPADO_x_STR){
			leerContenidoPosicion(adm.frame, contenido);
			log_info(archivoLog,"\n***PID = %d -- PAGINA = %d -- FRAME = %d -- CONTENIDO = %s ***",adm.pid,adm.nroPagina,adm.frame,(char *) contenido);

		}
		c++;
	}
	log_info(archivoLog,"*** FIN ***");

}

void leerContenidoPosicion(int frame,void* contenido){

	int movimiento =  _frameSize+getSizeStrAdm();
	memcpy(contenido,ptrMemoria+frame*movimiento, _frameSize);

}

void verContenidoDeProceso(int pid){
	strAdministrativa adm;
	void *contenido = malloc(_frameSize);
	int c=0;
	log_info(archivoLog,"*** SOLICITUD DE CONTENIDO //PID = %d // ***",pid);
	while(c<_frames){
		memcpy(&adm, ptrMemoria+c*getSizeStrAdm(),getSizeStrAdm());
		if(adm.pid==pid){
			memcpy(contenido,ptrMemoria+adm.frame*_frameSize,_frameSize);
			log_info(archivoLog,"/// Nro Pagina = %d /// Frame = %d ///\nContenido = %s", adm.nroPagina, adm.frame, (char*) contenido);
		}
		c++;
	}
	log_info(archivoLog,"*** FIN ***");

	free(contenido);
}


void contenidoDeMemoria(){
	int opcion=0,pid=-1;

	printf("Seleccione una de las siguientes opciones \n1.Ver contenido de toda la memoria. \n2.Ver contenido de un proceso.");
	switch(opcion){
	case 1:
		verContenidoDeMemoria();
		break;
	case 2 :
		printf("Ingrese el PID, del cual quiere saber su contenido\n");
		printf("%d",pid);
		verContenidoDeProceso(pid);
		break;
	default:
		printf("Error en el ingreso de una opcion");
		break;
	}


}


//Genera un reporte en pantalla y en un archivo en disco del estado actual del
//cache, estructuras de memoria, contenido de memoria
void dump() {
	int opcion=0;
		menuDump();
		scanf("\n%d",&opcion);
		switch(opcion){
		case 1:
			imprimirEntradasActivas();
			break;
		case 2:
			imprimirStrAdm(ptrMemoria);
			imprimirEntradasSinContenido();
			break;
		case 3:
			contenidoDeMemoria();
			break;
		default:
			break;

		}

}

//Este comando debera limpiar el contenido de la cache
void flush() {
	int opcion=0;
	printf("\nEsta seguro de que desea limpiar el contenido de la cache.\n1.Si\n2.No");
	scanf("%d",&opcion);
	if(opcion==1){
		iniciarAdmCache();
	}
}

//MEMORY: Indicara el tamaño de la memoria en cant. de frames, la cant. de frames ocupados y los libres
//PID: Indicara el tamaño total de un procoeso
void size() {
	int opcion=0,pid=-1,tamanoPrc;
	printf("Ingrese una de las siguientes opciones. \n1.Tamano de un proceso.\n2.Tamano de la memoria");
	switch(opcion){
	case 1:
		log_info(archivoLog, "*** TAMANO DE UN PROCESO ***");
		printf("Ingrese el PID del proceso");
		scanf("%d",&pid);
		tamanoPrc = paginasPrc(pid,ptrMemoria);
		log_info(archivoLog, "/// PID = %d /// FRAMES OCUPADOS = %d /// CANT BYTES OCUPADOS = %d ///",pid,tamanoPrc,tamanoPrc*_frameSize);
		log_info(archivoLog,"*** FIN ***");
		break;
	case 2:
		log_info(archivoLog, "*** TAMANO DE LA MEMORIA ***");
		log_info(archivoLog, "/// CANTIDAD DE FRAMES = %d /// CANTIDAD FRAMES OCUPADOS = %d /// CANTIDAD FRAMES LIBRES = %d ///",
				_frames,paginasOcupadasEnMemoria(),paginasLibresEnMemoria(ptrMemoria));
		log_info(archivoLog, "*** FIN ***");
		break;
	default:
		break;
	}

}

void consolaMemoria(){
	int opcion=0,retardo=0;
	printf("Bienvenido a la consola de la memoria porfavor ingrese una de la siguientes opciones");
	menuConsola();
	scanf("\n%d",&opcion);
	while(opcion!=5){
		switch(opcion){

		case 1:
			printf("Ingrese el nuevo retardo en SEGUNDOS\n");
			scanf("\n%d",&retardo);
			modificarRetardo(retardo);
			break;
		case 2:
			dump();
			break;
		case 3:
			flush();
			break;
		case 4:
			size();
			break;
		default:
			printf("Operacion invalida. Reingrese nuevamente la opcion.");
			break;

		}
		menuConsola();
		scanf("\n%d",&opcion);

	}

}



//  ******* ENVIAR Y RECIBIR DATOS  *******
int recibirSeleccionOperacion(int socket) {
	int *operacion=0;
	read(socket,operacion,sizeof(int));
	printf("Orden de operacion recibido:  %d",*operacion);
	return *operacion;
}

void recibirDatosPID_PAG(int socket, int *pid, int *pag){
	recibirPaquete(socket, pid, sizeof(u_int32_t));
	recibirPaquete(socket, pag, sizeof(u_int32_t));
}

void recibirDatosTotal(int socket, int* pid, int *pag, int* offset, int *size){
	recibirDatosPID_PAG(socket, pid, pag);
	recibirPaquete(socket, &offset, sizeof(u_int32_t));
	recibirPaquete(socket, &size, sizeof(u_int32_t));

}

void enviarMensaje(int socket, void* buffer, size_t tamanioBuffer){

	paquete* pack =  crearPaquete(tamanioBuffer);
	empaquetarVariable(pack, buffer,tamanioBuffer);
}



// ******* GETS *******

int getPuertoMemoria(){
	return _puertoMemoria;
}
int getFrameSize(){
	return _frameSize;
}

int getSizeStrAdm(){
	return sizeof(strAdministrativa);
}

int getRetardo(){
	return _retardoMemoria;
}




#endif /* FUNCIONESMEMORIA_H_ */

