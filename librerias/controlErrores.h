/*
 * controlErrores.h
 *
 *  Created on: 17/4/2017
 *      Author: Zero_Gravity
 */

#ifndef CONTROLERRORES_H_
#define CONTROLERRORES_H_

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void esErrorSimple(int valorRtaFuncion, char* mensaje){
	if(valorRtaFuncion<0){
		perror(mensaje);
		exit(EXIT_FAILURE);
	}
}

void esErrorConSalida(int valorRtaFuncion, char* mensajeError){
	if (valorRtaFuncion == -1){
		perror(mensajeError);
		exit(EXIT_FAILURE);
	}	
}

void esErrorSinSalida(int valorRtaFuncion, char* mensajeError){
	if (valorRtaFuncion == -1){
		perror(mensajeError);
	}
}

void errorSalidaSocket(int valorRtaFuncion, char* mensajeError, int socket){
	if (valorRtaFuncion == -1){
		perror(mensajeError);
		close(socket);
		exit(EXIT_FAILURE);
	}
}


void sinBytesRecibidos(int bytesRecibidos){
	if(bytesRecibidos == 0)	{
		perror("El kernel se ha desconectado");
		exit(EXIT_FAILURE);
	}
}

#endif /* CONTROLERRORES_H_ */
