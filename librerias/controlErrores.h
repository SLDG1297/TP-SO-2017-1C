/*
 * controlErrores.h
 *
 *  Created on: 17/4/2017
 *      Author: Zero_Gravity
 */

#ifndef CONTROLERRORES_H_
#define CONTROLERRORES_H_

#include <stdlib.h>

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

void errorSalidaSocket(int valorRtaFuncion, char* mensajeError, int* socket){
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
