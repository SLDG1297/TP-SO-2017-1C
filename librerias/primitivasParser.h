/*
 * primitivasParser.h
 *
 *  Created on: 3/5/2017
 *      Author: utnso
 */

#ifndef PRIMITIVASPARSER_H_
#define PRIMITIVASPARSER_H_

#include <commons/collections/list.h>

#include <parser/metadata_program.h>
#include <parser/sintax.h>
#include <parser/parser.h>
#include "../librerias/funcionesCPU.h"
#include "../librerias/pcb.h"

//Funciones generales

pcb* PCB;
u_int32_t* TAMANIO_PAG;

//Funcion para tener cargado el pcb
void llenarVarsPrimitivas(pcb* _PCB, u_int32_t* _TAMANIO_PAG);

//Funcion para calcular el t_puntero a partir de una posicion de memoria
t_puntero calculart_puntero(posicionMemoria posMem);

//Dado una dirección como numPag*TAMAÑO_PAG + offset calcula buscando en el stack(cualquier contexto) su correspondiente posicion de memoria
posicionMemoria* obtenerPosicionMemoria(t_puntero direccion);

void llenarPCB(pcb* _PCB, u_int32_t* _TAMANIO_PAG){
	PCB = _PCB;
	TAMANIO_PAG = _TAMANIO_PAG;
}

t_puntero calculart_puntero(posicionMemoria posMem){
	u_int32_t tamanioPag = *TAMANIO_PAG; // Te pongo esto porque lo otro era antipedagógico: confundía punteros con un producto XD
	return (posMem.pagina)*tamanioPag + posMem.offset;
}

posicionMemoria* obtenerPosicionMemoria(t_puntero direccion){
	t_list* stack = PCB->indiceStack;
	int cantidadDeContextos = stack->elements_count;

	//For para ir cambiando de contexto del stack
	for(int i = 0;i < cantidadDeContextos;i++){
    indiceDeStack* aux = (indiceDeStack*)list_get(stack,i);
    int cantArgs = aux->argumentos->elements_count;
    int cantVars = aux->variables->elements_count;

    //For para ver lista de argumentos
    for(int j = 0;j < cantArgs ;j++){
    argStack* argStack = (argStack*)list_get(aux->argumentos,j);

    //Si encontró la dirección correcta
    if(direccion == calculart_puntero(*argStack)){
    	return (posicionMemoria*)argStack;
    }
    }/*Fin del for de lista de argumentos*/

    //For para ver lista de variables
    for(int k = 0;k < cantVars;k++){
    variableStack* varStack = (variableStack*)list_get(aux->variables,k);

    //Si encontró la dirección correcta
    if(direccion == calculart_puntero(*varStack)){ // Estás flasheando polimorfismo paramétrico XD Ahí va posicionMemoria, no varStack XD
        	return (posicionMemoria*)varStack;
    }
    }/*Fin del for de lista de variables*/

	}/*Fin del for principal(el del contexto)*/
	return NULL;
}


//OPERACIONES PARA ESTRUCTURA AnSISOP_funciones -----------------------------------------------------------------------

//Declaraciones

//Reserva el espacio para una variable
t_puntero definirVariable(t_nombre_variable identificador_variable);

//Devuelve la posicion en memoria (proceso) de la variable
t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable);

//Devuelve el valor asociado a la posicion de memoria(variable) alojada en direccion_variable
t_valor_variable dereferenciar(t_puntero direccion_variable);

//Dada tal direccion_variable, se busca su posicion en el proceso memoria y se escribe el valor correspondiente
void asignar(t_puntero direccion_variable, t_valor_variable valor);

//Se obtiene el valor de determinada variable compartida
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable);

//Se asigna un valor a determinada variable compartida
t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor);

//Cambia la linea de ejecucion a la correspondiente de la etiqueta buscada
void irAlLabel(t_nombre_etiqueta t_nombre_etiqueta);

void llamarSinRetorno(t_nombre_etiqueta etiqueta);

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);

void finalizar(void);

void retornar(t_valor_variable retorno);


//Definiciones

//-------------------------------------------------------------------------Falta considerar lo del diccionario de variables
t_puntero definirVariable(t_nombre_variable identificador_variable){

	posicionMemoria posMem /*= solicitarEspacioAMemoria(tamañoDeVariable)*/;
	t_list* stack = PCB->indiceStack;
	//Indice de la última posición para pararme en el contexto actual
	int ultPos = (stack->elements_count) - 1;

	indiceDeStack* aux = (indiceDeStack*)list_get(stack,ultPos);

	//Para ver si es el argumento de una funcion
	if (identificador_variable >= '0' && identificador_variable <= '9'){
    argStack* nuevoArgumento;
    *nuevoArgumento = posMem;
    list_add(aux->argumentos,nuevoArgumento);
	}

	//En caso contrario, se trata de una variable normal
	else{
	variableStack* nuevaVariable;
	nuevaVariable->nombre = identificador_variable;
	nuevaVariable->posicionMemoria = posMem;
	list_add(aux->variables,nuevaVariable);
	}

	return calculart_puntero(posMem);
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){

	t_list* stack = PCB->indiceStack;
	//Indice de la última posición para pararme en el contexto actual
	int ultPos = (stack->elements_count) - 1;

	indiceDeStack* aux = (indiceDeStack*)list_get(stack,ultPos);

	//Ahora tengo que recorrer la lista de variables hasta encontrar la requerida
	t_list* listaVariables = aux->variables;
	t_list* pr = listaVariables;
	while(((variableStack*)pr->head->data)->nombre != identificador_variable && pr->head->next != NULL){
		pr = pr->head->next;
	}

	//Si encontró la variable devuelve la posición de memoria en donde está la página, offset y size
	if(((variableStack*)pr->head->data)->nombre == identificador_variable){
		posicionMemoria posMem = ((variableStack*)pr->head->data)->posicionMemoria;
		return calculart_puntero(posMem);
	}

	//Si no estaba la variable en la lista
    return -1;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){

	posicionMemoria* posMem = obtenerPosicionMemoria(direccion_variable);
    u_int32_t pagina = posMem->pagina;
    u_int32_t offset = posMem->offset;
    u_int32_t size = posMem->size;
    t_valor_variable valor /*= solicitarMemoria(pagina, offset, size)*/;
    return valor;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor){
	posicionMemoria* posMem = obtenerPosicionMemoria(direccion_variable);
	u_int32_t pagina = posMem->pagina;
	u_int32_t offset = posMem->offset;
	u_int32_t size = posMem->size;
	//Se le pide escritura a la memoria en la posicion determinada
	//escribirValorEnMemoria(pagina, offset, size, valor);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){
	t_valor_variable valor /*= pedirKernelVariableCompartida(variable)*/;
	return valor;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	t_valor_variable valorAsignado /*= asignarKernelVariableCompartida(variable,valor)*/;
	return valorAsignado;
}

void irAlLabel(t_nombre_etiqueta t_nombre_etiqueta){

}

//-----------------------------------------------------------------------------------------------------------------------

//OPERACIONES DE KERNEL PARA ESTRUCTURA AnSISOP_kernel ------------------------------------------------------------------

//Declaraciones

void wait(t_nombre_semaforo identificador_semaforo);

void signal(t_nombre_semaforo identificador_semaforo);

t_puntero reservar(t_valor_variable espacio);

void liberar(t_puntero puntero);

t_descriptor_archivo abrir(t_direccion_archivo direccion, t_banderas flags);

void borrar(t_descriptor_archivo direccion);

void cerrar(t_descriptor_archivo descriptor_archivo);

void moverCursor(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion);

void escribir(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio);

void leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio);

//Definiciones




//-----------------------------------------------------------------------------------------------------------------------

#endif /* PRIMITIVASPARSER_H_ */
