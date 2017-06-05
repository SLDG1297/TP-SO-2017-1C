/*
 * conexionSimpleServidor.c
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#include "../librerias/conexionSocket.h"

int main(){
	int puertoServidor = 5100;

	int socketCliente = servir(puertoServidor);

	u_int32_t datoRecibido;

	recv(socketCliente, &datoRecibido, sizeof(u_int32_t), 0);

	if(datoRecibido == 4)
		printf("Se recibió el dato correctamente");
	else
		printf("Que en paz descanses...");

}
