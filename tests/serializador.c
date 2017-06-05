#include <stdio.h>
#include <stdbool.h>
#include <cspecs/cspec.h>

#include "../librerias/serializador.h"

// Script para compilar:
// gcc serializador.c -o serializador -lcommons -lcspecs

context (serializador){

	describe ("Empaquetado de datos únicos"){
		u_int32_t paquete = 200;
		size_t tamanioPaquete = sizeof(u_int32_t);
		void* mensaje;

		before{
			mensaje = crearPaquete(tamanioPaquete);
			empaquetar(mensaje, &paquete, tamanioPaquete);
		} end

		after{
			destruirPaquete(mensaje);
		} end

		it("Puedo desempaquetar el dato y obtener el valor 200"){
			referenciarInicio(mensaje, tamanioPaquete);
			u_int32_t receptor;

			desempaquetar(&receptor, mensaje, tamanioPaquete);
			should_int(200) be equal to(receptor);
		} end

		it("Si no hubiera referenciado al inicio del mensaje, no pasaba nada"){
			u_int32_t receptor;

			desempaquetar(&receptor, mensaje, tamanioPaquete);
			should_int(200) be equal to(receptor);
		} end

	} end

	describe("Empaquetado de estructuras compuestas"){
		struct registroConPadding{
			u_int32_t 	dato1;
			char 		dato2;
		};

		struct registro{
			u_int32_t 	dato1;
			char 		dato2;
		}__attribute__((packed));

		struct registro dato;
		dato.dato1 = 10;
		dato.dato2 = 'a';

		struct registro receptor;

		void* mensaje;

		size_t tamanio = sizeof(struct registro);

		before{
			mensaje = crearPaquete(tamanio);
			empaquetar(mensaje, &dato.dato1, sizeof(u_int32_t));
			empaquetar(mensaje, &dato.dato2, sizeof(char));
		} end

		after{
			destruirPaquete(mensaje);
		} end

		it("Tamaño de estructura tiene que ser igual a la suma de sus partes"){
			size_t tamanioReal = sizeof(u_int32_t) + sizeof(char);

			should_int(tamanioReal) be equal to (tamanio);
		} end

		it("Tamaño de estructura sin padding debería ser distinto al que la tenga"){
			size_t tamanioConPadding = sizeof(struct registroConPadding);

			should_int(tamanioConPadding) not be equal to (tamanio);
		} end

		it("Puedo desempaquetar la estructura y obtener el entero 10"){
			desempaquetar(&receptor.dato1, mensaje, sizeof(u_int32_t));

			should_int(receptor.dato1) be equal to (10);
		} end

		it("Puedo desempaquetar la estructura y obtener el caracter 'a'"){
			desempaquetar(&receptor.dato1, mensaje, sizeof(u_int32_t));
			desempaquetar(&receptor.dato2, mensaje, sizeof(char));

			should_char(receptor.dato2) be equal to ('a');
		} end

	} end
}
