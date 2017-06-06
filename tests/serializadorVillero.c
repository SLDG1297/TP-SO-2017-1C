#include <stdio.h>
#include <stdbool.h>
#include <commons/string.h>
#include <cspecs/cspec.h>

#include "../librerias/serializador.h"

// Script para compilar:
// gcc serializadorVillero.c -o serializadorVillero -lcommons -lcspecs

context (serializador){

	describe ("Empaquetado de datos únicos."){
		u_int32_t paquete = 200;
		size_t tamanio = sizeof(u_int32_t);

		void* envio = malloc(tamanio);
		void* recibo = malloc(tamanio);

		before{
			memcpy(envio, &paquete, tamanio);

			memcpy(recibo, envio, tamanio);
		} end

		after{
			free(envio);

			free(recibo);
		} end

		it("Puedo desempaquetar el dato y obtener el valor 200."){
			u_int32_t receptor;

			memcpy(&receptor, envio, tamanio);
			should_int(200) be equal to(receptor);
		} end

	} end

	describe("Empaquetado de estructuras compuestas."){
		struct registro{
			u_int32_t 	dato1;
			float		dato2;
			double		dato3;
			char		dato4;
		}__attribute__((packed));

		struct registroConPadding{
			u_int32_t 	dato1;
			float		dato2;
			double		dato3;
			char		dato4;
		};

		struct otro{
			int a;
			double b;
		}__attribute__((packed));

		struct otroConPadding{
			int a;
			double b;
		};

		struct registro dato;
		dato.dato1 = 10;
		dato.dato2 = 30.0;
		dato.dato3 = 52146.7;
		dato.dato4 = 'w';

		struct registro receptor;

		size_t tamanio = sizeof(struct registro);

		before{
			// Creación de paquete
			void* envio = malloc(tamanio);

			// Empaquetado
			memcpy(envio, &dato.dato1, sizeof(u_int32_t));
			envio += sizeof(u_int32_t);

			memcpy(envio, &dato.dato2, sizeof(float));
			envio += sizeof(float);

			memcpy(envio, &dato.dato3, sizeof(double));
			envio += sizeof(double);

			memcpy(envio, &dato.dato4, sizeof(char));
			envio += sizeof(char);

			// Referenciar a inicio para enviar bien las cosas.
			envio -= tamanio;

			// Se supone que se envía con un send desde el proceso que empaqueta el mensaje y se libera el envio.

			// Se supone que acá debe haber un recv en otro proceso.

			// Recibo de paquete

			void* recibo = malloc(tamanio);
			memcpy(recibo, envio, tamanio);

			free(envio); // Libero el envío porque ya no lo necesito.d

			// Desempaquetado
			memcpy(&receptor.dato1, recibo, sizeof(u_int32_t));
			recibo += sizeof(u_int32_t);

			memcpy(&receptor.dato2, recibo, sizeof(float));
			recibo += sizeof(float);

			memcpy(&receptor.dato3, recibo, sizeof(double));
			recibo += sizeof(double);

			memcpy(&receptor.dato4, recibo, sizeof(char));
			recibo += sizeof(char);

			// Referenciar a inicio para poder liberar el recibo correctamente.
			recibo -= tamanio;
			free(recibo);
		} end

		it("Puedo desempaquetar la estructura y obtener el entero 10."){
			should_int(receptor.dato1) be equal to (10);
		} end

		it("Puedo desempaquetar la estructura y obtener el flotante 30.0."){
			should_float(receptor.dato2) be equal to (30.0);
		} end

		it("Puedo desempaquetar la estructura y obtener el double 52146.7."){
			should_double(receptor.dato3) be equal to (52146.7);
		} end

		it("Puedo desempaquetar la estructura y obtener el caracter 'w'."){
			should_char(receptor.dato4) be equal to ('w');
		} end

		it("Tamaño de estructura tiene que ser igual a la suma de sus partes."){
			size_t tamanioReal = sizeof(u_int32_t) + sizeof(float) + sizeof(double) + sizeof(char);

			should_int(tamanioReal) be equal to (tamanio);
		} end

		it("Tamaño de estructura con padding debería ser mayor o igual al que no tenga."){
			size_t tamanioConPadding = sizeof(struct registroConPadding);
			bool proposicion1 = tamanioConPadding >= tamanio;

			bool proposicion2 = sizeof(struct otro) >= sizeof(struct otroConPadding);

			bool proposicion = proposicion1 && proposicion2;

			should_bool(proposicion) be equal to (true);
		} end

	} end
	/* OTRO DÍA VEO ESTO PORQUE ME TIENE PODRIDO.
	describe("Empaquetado de estructuras variables."){
		char* receptor;

		before{
			// Contexto del proceso emisor:
			char* dato = "RompiendoTodo";
			size_t tamanio = string_length(dato);

			size_t tamanioEnvio = tamanio + sizeof(size_t);

			// Empaquetado
			void* envio = malloc(tamanioEnvio);

			memcpy(envio, &tamanio, sizeof(size_t));
			envio += sizeof(size_t);

			memcpy(envio, dato, tamanio);
			envio += tamanio;

			envio -= tamanioEnvio;

			// Enviar envio (Duh!)


			// Contexto del proceso receptor:

			size_t tamanioRecibo;

			// Recibir envio
			memcpy(&tamanioRecibo, envio, sizeof(size_t));
			envio += sizeof(size_t);

			void* recibo = malloc(tamanioRecibo);
			memcpy(recibo, envio, tamanioRecibo);

			free(envio);

			memcpy(receptor, recibo, tamanioRecibo);

			recibo -= tamanioRecibo;
		} end

		it("Se puede desempaquetar 'RompiendoTodo', string de tamaño 12."){
			should_string(receptor) be equal to ("RompiendoTodo");
		} end

	} end*/
}
