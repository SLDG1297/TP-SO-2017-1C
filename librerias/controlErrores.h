/*
 * controlErrores.h
 *
 *  Created on: 17/4/2017
 *      Author: Zero_Gravity
 */

#ifndef CONTROLERRORES_H_
#define CONTROLERRORES_H_

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

void sinBytesRecibidos(int bytesRecibidos){
	if(bytesRecibidos == 0)	{
		perror("El kernel se ha desconectado");
		exit(EXIT_FAILURE);
	}
}



#endif /* CONTROLERRORES_H_ */
