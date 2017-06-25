#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <commons/collections/list.h>



#include "../librerias/pcb.h"



// Variables globales

char* ruta = "crearPCBCodigo.txt";



// Declaraciones

pcb 		crearPCB(u_int32_t pid, char* path, u_int32_t tamanioPaginas);

char* 		obtenerCodigo(char* path);

u_int32_t	calcularPaginasUsadas(u_int32_t tamanioInstrucciones, u_int32_t tamanioPaginas);



// Definiciones

pcb crearPCB(u_int32_t pid, char* path, u_int32_t tamanioPaginas){
	pcb nuevoProceso;							// Inicializar PCB

	nuevoProceso.pid = pid;						// Asignar PID

	char* codigo = obtenerCodigo(path);			// El File System devuelve el código de un archivo.

	preprocesador(codigo, &nuevoProceso);		// Obtengo los índices de etiquetas y de código del código.

	nuevoProceso.paginasUsadas = calcularPaginasUsadas(nuevoProceso.cantidadInstrucciones, tamanioPaginas);	// Asignar páginas usadas por el código.

	nuevoProceso.indiceStack = list_create();	// Inicializar stack;

	free(codigo);

	return nuevoProceso;
}

char* obtenerCodigo(char* path){
	char* codigo;
	long int tamanio;

	FILE* archivo = fopen(path, "r");

	fseek(archivo, 0, SEEK_END);
	tamanio = ftell(archivo);
	rewind(archivo);

	codigo = malloc(tamanio);
	fread(codigo, sizeof(char), tamanio, archivo);

	fclose(archivo);

	return codigo;
}

u_int32_t calcularPaginasUsadas(u_int32_t tamanioInstrucciones, u_int32_t tamanioPaginas){
	return tamanioInstrucciones / tamanioPaginas;
}



int main(){
	printf("Test para crear PCB\n\n");

	char* codigo = obtenerCodigo(ruta);

	printf("Código:\n%s\n\n", codigo);

	pcb proceso = crearPCB(20, ruta, 256);

	printf("Estado del PCB\nPID = %d\n", proceso.pid);

	printf("Program Counter = %d\n", proceso.programCounter);

	printf("Páginas usadas = %d\n", proceso.paginasUsadas);

	printf("Cantidad instrucciones = %d\n", proceso.cantidadInstrucciones);

	printf("Tamaño instrucciones = %d\n", proceso.cantidadInstrucciones * sizeof(lineaUtil));

	printf("Indice de Codigo:\n");

	int i = 0;
	while(i < proceso.cantidadInstrucciones)
	{
		printf("Instruccion %.2d: [Longitud = %.3d][Offset = %.3d]\n", i, proceso.indiceCodigo[i].longitud, proceso.indiceCodigo[i].offset);
		i++;
	};

	printf("Tamanio de indice de etiquetas = %d\n", proceso.bytesEtiquetas);

	printf("Indice de etiquetas = ");
	i = 0;
	while(i < proceso.bytesEtiquetas)
	{
		printf("%c", proceso.indiceEtiqueta[i]);
		i++;
	};

	printf("\nAnda re piola XD\n");

	return 0;
}
