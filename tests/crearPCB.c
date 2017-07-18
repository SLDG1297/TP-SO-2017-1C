#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <commons/collections/list.h>

#include "../librerias/pcb.h"



int main(){
	printf("Test para crear PCB\n\n");

	char* ruta = "crearPCBCodigo.txt";

	char* codigo = obtenerCodigo(ruta);

	printf("Código:\n%s\n\n", codigo);

	free(codigo);

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

	free(proceso.indiceCodigo);
	free(proceso.indiceEtiqueta);

	return 0;
}
