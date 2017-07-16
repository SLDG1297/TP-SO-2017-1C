#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <commons/txt.h>

#include "../librerias/pcb.h"
#include "../librerias/fileSystem/funcionesFileSystem.h"

void prueba();

void prueba(){
	int i;
	pcb* unPcb = malloc(sizeof(pcb));
	char* codigo = "\nbegin\n	funcion1\nend\n\nfunction funcion1\n	variables a\n	a=1\n	print a\n	funcion2\nend\n\nfunction funcion2\n	variables a\n	a=0\n	print a\n	funcion1\nend";
	//Sorry que sea tan molesto, pero traté de pasarlo de un .txt a string y ripié. Más adelante pruebo.

	printf("Programa a preprocesar:\n%s\n", codigo);

	preprocesador(codigo, unPcb);

	printf("\nResultados:\n");

	printf("Program Counter = %d\n", unPcb->programCounter);

	printf("Cantidad de instrucciones = %d\n", unPcb->cantidadInstrucciones);

	printf("Tamanio de Instrucciones = %d\n", unPcb->cantidadInstrucciones * sizeof(lineaUtil));

	printf("Indice de Codigo:\n");
	i = 0;
	while(i < unPcb->cantidadInstrucciones)
	{
		printf("Instruccion %.2d: [Longitud = %.3d][Offset = %.3d]\n", i, unPcb->indiceCodigo[i].longitud, unPcb->indiceCodigo[i].offset);
		i++;
	};

	printf("Tamanio de indice de etiquetas = %d\n", unPcb->bytesEtiquetas);

	printf("Indice de etiquetas = ");
	i = 0;
	while(i < unPcb->bytesEtiquetas)
	{
		printf("%c", unPcb->indiceEtiqueta[i]);
		i++;
	}

	printf("\nNo se ve bien porque deben haber ints hardcodeados y los caracteres '0'.\n");

	free(unPcb->indiceCodigo);
	free(unPcb->indiceEtiqueta);
	free(unPcb);
}

int main(){
	printf("Test del preprocesador:\n\n");

	prueba();

	return 0;
}
