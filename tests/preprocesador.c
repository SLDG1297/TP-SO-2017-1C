#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <commons/txt.h>

#include "../librerias/pcb.h"

char* obtenerCodigo(char* path);

void prueba1();

void prueba2();

void prueba1(){
	printf("Prueba del preprocesador sin abstraer:\n\n");

	size_t tamanio;

	pcb unPcb;

	char* codigo = "\nbegin\n	variables a, b\n	a = 3\n	b = 5\n	a = b + 12\nend";

	t_metadata_program* programa = metadata_desde_literal(codigo);

	printf("Programa a preprocesar:\n%s\n", codigo);

	printf("\nResultados:\n");

	unPcb.programCounter = programa->instruccion_inicio;

	printf("Program Counter = %d\n", unPcb.programCounter);

	unPcb.cantidadInstrucciones = programa->instrucciones_size;

	printf("Cantidad de instrucciones = %d\n", unPcb.cantidadInstrucciones);

	tamanio = unPcb.cantidadInstrucciones * sizeof(lineaUtil);

	printf("Tamanio de Instrucciones = %d\n", tamanio);

	unPcb.indiceCodigo = malloc(tamanio);
	memcpy(unPcb.indiceCodigo, programa->instrucciones_serializado, tamanio);

	printf("Indice de Codigo:\n");
	int i = 0;
	while(i < unPcb.cantidadInstrucciones) // Uso un while porque andan diciendo que no se pueden usar fors, jaja saludos...
	{
		printf("Instruccion %d: [Longitud = %.2d][Offset = %.2d]\n", i, unPcb.indiceCodigo[i].longitud, unPcb.indiceCodigo[i].offset);
		i++;
	};

	unPcb.bytesEtiquetas = programa->etiquetas_size;

	printf("Cantidad de etiquetas = %d\n", unPcb.bytesEtiquetas);

	tamanio = unPcb.bytesEtiquetas;
	unPcb.indiceEtiqueta = malloc(tamanio);
	memcpy(unPcb.indiceEtiqueta, programa->etiquetas, tamanio);

	printf("Indice de etiquetas = %s\n", unPcb.indiceEtiqueta);

	metadata_destruir(programa);

	free(unPcb.indiceCodigo);
	free(unPcb.indiceEtiqueta);
}

void prueba2(){
	printf("\n\nPrueba del preprocesador con abstracción:\n\n");

	pcb* unPcb = malloc(sizeof(pcb));
	char* codigo = "\nbegin\n	f\nend\n\nfunction f\n	variables a\n	a=1\n	print a\n	g\nend\n\nfunction g\n	variables a\n	a=0\n	print a\n	f\nend";
	//Sorry que sea tan molesto, pero traté de pasarlo de un .txt a string y ripié. Más adelante pruebo.

	printf("Programa a preprocesar:\n%s\n", codigo);

	preprocesador(codigo, unPcb);

	printf("\nResultados:\n");

	printf("Program Counter = %d\n", unPcb->programCounter);

	printf("Cantidad de instrucciones = %d\n", unPcb->cantidadInstrucciones);

	printf("Tamanio de Instrucciones = %d\n", unPcb->cantidadInstrucciones * sizeof(lineaUtil));

	printf("Indice de Codigo:\n");
	int i = 0;
	while(i < unPcb->cantidadInstrucciones)
	{
		printf("Instruccion %.2d: [Longitud = %.3d][Offset = %.3d]\n", i, unPcb->indiceCodigo[i].longitud, unPcb->indiceCodigo[i].offset);
		i++;
	};

	printf("Indice de etiquetas = %s\n", unPcb->indiceEtiqueta);

	free(unPcb->indiceCodigo);
	free(unPcb->indiceEtiqueta);
	free(unPcb);
}

int main(){
	printf("Tests del preprocesador:\n\n");

	prueba1();

	prueba2();

	return 0;
}
