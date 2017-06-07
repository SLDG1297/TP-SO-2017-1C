#include <stdio.h>
#include <stdbool.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <cspecs/cspec.h>

#include "../librerias/serializador.h"

// Script para compilar:
// gcc serializadorBeta.c -o serializadorBeta -lcommons -lcspecs

context (serializador){

	describe ("Empaquetado de datos únicos:"){
		it("Se tiene que desempaquetar correctamente el entero 200."){

			// Contexto del emisor:
			u_int32_t emisor = 200;

			// Empaquetado
			void* envio = malloc(sizeof(u_int32_t));

			memcpy(envio, &emisor, sizeof(u_int32_t));

			// >> Envío


			// Contexto del receptor:
			u_int32_t receptor;

			// << Recibo
			void* recibo = malloc(sizeof(u_int32_t));

			memcpy(recibo, envio, sizeof(u_int32_t));

			// Desempaquetado
			memcpy(&receptor, recibo, sizeof(u_int32_t));

			free(envio);
			free(recibo);

			should_int(receptor) be equal to (emisor);

		} end
	} end

	describe("Empaquetado de datos variables:"){
		it("Se tiene que poder desempaquetar correctamente la cadena 'Rotisimo'."){

			// Contexto del emisor:

			char* emisor = "Rotisimo";
			u_int32_t tamanioEmisor = strlen(emisor) + 1;

			// Empaquetado

			void* envio = malloc(sizeof(u_int32_t) + tamanioEmisor);

			memcpy(envio, &tamanioEmisor, sizeof(u_int32_t));
			memcpy(envio + sizeof(u_int32_t), emisor, tamanioEmisor);

			// >> Envío

			// Contexto del receptor:

			char* receptor;
			u_int32_t tamanioReceptor;

			// << Recibo

			memcpy(&tamanioReceptor, envio, sizeof(u_int32_t));

			void* recibo = malloc(tamanioReceptor);

			// Desempaquetado

			memcpy(recibo, envio + sizeof(u_int32_t), tamanioReceptor);

			receptor = malloc(tamanioReceptor);
			memcpy(receptor, recibo, tamanioReceptor);

			free(envio);
			free(recibo);

			should_int(tamanioReceptor) be equal to (tamanioEmisor);
			should_string(receptor) be equal to (emisor);

		} end
	} end

	describe("Empaquetado de estructuras:"){
		struct estructura{
			u_int32_t numero;
			char letra;
		} __attribute__((packed));

		struct estructuraConPadding{
			u_int32_t numero;
			char letra;
		};

		it("El tamaño de la estructura sin Padding debe ser menor o igual a la que sí tiene."){
			u_int32_t tamanioSinPadding = sizeof(struct estructura);
			u_int32_t tamanioConPadding = sizeof(struct estructuraConPadding);

			bool proposicion = tamanioSinPadding <= tamanioConPadding;

			should_bool(proposicion) be equal to (true);
		} end

		it("Se tienen que desempaquetar de la estructura los valores 5 y 'b'."){
			// Contexto del emisor:

			struct estructura emisor;
			emisor.numero = 5;
			emisor.letra = 'b';

			u_int32_t tamanioEnvio = sizeof(struct estructura);

			// Empaquetado

			void* envio = malloc(tamanioEnvio);
			u_int32_t desplazamientoEnvio = 0;

			memcpy(envio + desplazamientoEnvio, &emisor.numero, sizeof(u_int32_t));
			desplazamientoEnvio += sizeof(u_int32_t);

			memcpy(envio + desplazamientoEnvio, &emisor.letra, sizeof(char));
			desplazamientoEnvio += sizeof(char);

			// >>Envío

			// Contexto del receptor:

			struct estructura receptor;

			u_int32_t tamanioRecibo = sizeof(struct estructura);

			// <<Recibo

			void* recibo = malloc(tamanioRecibo);
			u_int32_t desplazamientoRecibo = 0;

			// Desempaquetado

			memcpy(recibo, envio, tamanioRecibo);

			memcpy(&receptor, recibo, tamanioRecibo);

			memcpy(&receptor.numero, recibo + desplazamientoRecibo, sizeof(u_int32_t));
			desplazamientoRecibo += sizeof(u_int32_t);

			memcpy(&receptor.letra, recibo + desplazamientoRecibo, sizeof(char));
			desplazamientoRecibo += sizeof(char);

			free(envio);
			free(recibo);

			should_int(receptor.numero) be equal to (emisor.numero);
			should_char(receptor.letra) be equal to (emisor.letra);

		} end

	} end
}
