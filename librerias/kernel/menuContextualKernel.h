/*
 * menuContextualKernel.h
 *
 *  Created on: 6/6/2017
 *      Author: utnso
 */

#ifndef MENUCONTEXTUALKERNEL_H_
#define MENUCONTEXTUALKERNEL_H_

// MENU DE CONTEXTO
void generarMenu(void) {
	//Borrarpantalla clear??
	system("cls");
	printf("consola del kernel\n");
	printf("1-Listado de procesos en el sistema\n");
	printf("2-Acciones para un proceso determinado\n");
	printf("3-Tabla global de archivos\n");
	printf("4- Modificar grado de multiprogramacion\n");
	printf("5-Finalizar un proceso\n");
	printf("6-Detener la planificacion\n");
}

void generarMenuDeProceso(void) {
	printf("Elija una accion\n");
	printf("1- Cantidad de rafagas ejecutadas\n");
	printf("2-Cantidad de operaciones privilegiadas ejecutadas\n");
	printf("3-Tabla de archivos abiertos por el proceso\n");
	printf("4-Cantidad de paginas del heap utilizadas\n");
	printf("5-Cantidad de Syscalls ejecutadas\n");
}

void generarMenuDeHeap(void) {
	printf("1- cantidad de acciones alojar realizadas\n");
	printf("2- cantidad de acciones liberar realizadas\n");
}




#endif /* MENUCONTEXTUALKERNEL_H_ */
