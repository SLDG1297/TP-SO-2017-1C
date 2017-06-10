/*
 * protocolo.h
 *
 *  Created on: 10/6/2017
 *      Author: utnso
 */

#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

// Protocolos de comunicación del Sistema Operativo



// Protocolo de comunicación de las Consolas

// Protocolo de comunicación del Kernel

// Protocolo de comuniación del File System

// Protocolo de comunicación de la Memoria
#define INICIAR_PROGRAMA 		51
#define SOLICITAR_BYTES_PAG 	52
#define ALMACENAR_BYTES_PAG 	53
#define ASIGNAR_PAGINAS_PRC 	54
#define FINALIZAR_PRG 			55

// Protocolo de comunicación de las CPU's
#define	HANDSHAKE_CPU			1297
#define FIN_OPERACION 			1298
#define	FIN_PROGRAMA			1299
#define	EXCEPCION				1300
#define DESCONECTAR 			1301

#endif /* PROTOCOLO_H_ */
