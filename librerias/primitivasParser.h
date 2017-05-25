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
#include "../librerias/operacionesCPU.h"

//Funciones generales

pcb* PCB;

//Funcion para tener cargado el pcb
void llenarPCB(pcb* _PCB);

void llenarPCB(pcb* _PCB){
	PCB = _PCB;
}

//OPERACIONES PARA ESTRUCTURA AnSISOP_funciones -----------------------------------------------------------------------

//Declaraciones

//Reserva el espacio para una variable
t_puntero definirVariable(t_nombre_variable identificador_variable);

//Devuelve un puntero a la posicion de memoria en donde se guarda la pagina, offset y size de la variable
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
	variableStack nuevaVariable;
	nuevaVariable.nombre = identificador_variable;
	nuevaVariable.posicionMemoria = posMem;
	t_list* stack = PCB->indiceStack;
	t_list* pr = stack;
	//Recorro la lista para encontrar la ultima posicion del stack (contexto actual)
	while(pr->head->next != NULL){
		pr = pr->head->next;
	}
	indiceStack* aux= (indiceStack*)pr->head->data;
	int index = list_add(aux->variables,&nuevaVariable);
	//Por ahora asumo que el puntero es a la posicion de memoria en donde se encuentra la variable junto con su
	//correspondiente posicion en la memoria (proceso)
	variableStack* punteroAVar = (variableStack*)list_get(aux->variables,index);
	t_puntero posicion = &(punteroAVar->posicionMemoria);
	return posicion;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){
	t_list* stack = PCB->indiceStack;
	t_list* pr = stack;
	//Recorro la lista para encontrar la ultima posicion del stack (contexto actual)
	while(pr->head->next != NULL){
			pr = pr->head->next;
	}
	indiceStack* aux= (indiceStack*)pr->head->data;
	//Ahora tengo que recorrer la lista de variables hasta encontrar la requerida
	t_list* listaVariables = aux->variables;
	pr = listaVariables;
	while(((variableStack)pr->head->data)->nombre != identificador_variable && pr->head->next != NULL){
		pr = pr->head->next;
	}
	//Si encontró la variable devuelve la posicion de memoria en donde está la página, offset y size
	if(((variableStack)pr->head->data)->nombre == identificador_variable){
		posicionMemoria posMem = ((variableStack)pr->head->data)->posicionMemoria;
		return &posMem;
	}
	//Si no estaba la variable en la lista
    return -1;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){
    posicionMemoria* posMem = (posicionMemoria*)direccion_variable;
    u_int32_t pagina = posMem->pagina;
    u_int32_t offset = posMem->offset;
    u_int32_t size = posMem->size;
    //Se le manda la posicion de la variable para obtener su valor
    t_valor_variable valor /*= solicitarMemoria(pagina, offset, size)*/;
    return valor;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor){
	posicionMemoria* posMem = (posicionMemoria*)direccion_variable;
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
