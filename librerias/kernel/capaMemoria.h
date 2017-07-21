/*
 * capaMemoria.h
 *
 *  Created on: 20/7/2017
 *      Author: utnso
 */

#ifndef KERNEL_CAPAMEMORIA_H_
#define KERNEL_CAPAMEMORIA_H_

#include "../serializador.h"

#define INICIAR_PROGRAMA 	1051
#define SOLICITAR_BYTES_PAG 1052
#define ALMACENAR_BYTES_PAG 1053
#define ASIGNAR_PAGINAS_PRC 1054
#define FINALIZAR_PRC		1055
#define FINALIZAR_PRG		1056

extern int socketMemoria;
void iniciarPrograma(int pid,int nroPaginas){

}


#endif /* KERNEL_CAPAMEMORIA_H_ */
