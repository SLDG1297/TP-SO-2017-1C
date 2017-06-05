/*
 * conexionSimpleCliente.c
 *
 *  Created on: 4/6/2017
 *      Author: utnso
 */

#include "../librerias/conexionSocket.h"

int main(){
	char* ipServidor = "127.0.0.1";
	int puertoServidor = 5100;

	int socketServidor = conectar(ipServidor, puertoServidor);

	u_int32_t datoAEnviar = 4;

	send(socketServidor, &datoAEnviar, sizeof(u_int32_t), 0);

	printf("Se pudo enviar el mensaje correctamente.");
}
