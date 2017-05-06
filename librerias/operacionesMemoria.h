/*
 * operacionesMemoria.h
 *
 *  Created on: 30/4/2017
 *      Author: Zero Gravity
 */

#ifndef OPERACIONESMEMORIA_H_
#define OPERACIONESMEMORIA_H_

/*
 * Cuando el KRN comunique el inicio de un nuevo prog se crearan las estructuras necesarias para administrarlo correctamente. En una misma pag
 * no se podran tener datos referentes a dos segmentos diferentes
 */

//int inicializarPrograma(int pid, int cantPaginas){}


/*
 * Ante un pedido de lectura de paginas de alguno de los PRCs de la CPU, se realizara la traduccion a MARCO/FRAME y se devolvera el contenido
 * correspondiente consultado primeramente a la M. Cache, en caso de que esta no contenga la info se informara un Cache Miss, se debera cargar
 * la pagina en cache y se devolvera la informacion solicitada
 */
//char* solicitarBytesDePagina(int pid, int nroPagina, int offset, int size ){}

/*
 * Ante un pedido de escritura de paginas de alguno de los PRCDs, se realizara la traduccion a MARCO/FRAME y se actualizara su contenido
 * En caso de que la pag se encuentr en la M. Cache se debera actualizar tambien el frame alojado en la misma
 */
//int almacenarBytesEnPagina(int pid, int nroPagina, int offset, int size, char* buffer){}


/*
 *El PRC memoria debera aignarle tantas paginas como se soliciten, al PRC ampliando asi su tamaño. En caso de que no se
 *pueda asignar ese espacio, se debera informar al KRN la falta de espacio
 */
//int asignarPaginasAProceso(int pid, int paginasNecesarias){}


//Cuando el PRC KRN informe el fin de un prog. se deben eliminar las entradas en estructuras usadas de memoria
//int finalizarPrograma (int idPrograma){}


#endif /* OPERACIONESMEMORIA_H_ */
