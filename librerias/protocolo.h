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
#define VALIDAR_ARCHIVO			1701
#define CREAR_ARCHIVO			1702
#define BORRAR					1703
#define OBTENER_DATOS			1704
#define GUARDAR_DATOS			1705

// Protocolo de comunicación de la Memoria
#define INICIAR_PROGRAMA 		1051
#define SOLICITAR_BYTES_PAG 	1052
#define ALMACENAR_BYTES_PAG 	1053
#define ASIGNAR_PAGINAS_PRC 	1054
#define LIBERAR_PAG_1PRC		1055
#define FINALIZAR_PRG 			1056

// Protocolo de comunicación de las CPU's
#define	HANDSHAKE_CPU			1297
#define FIN_OPERACION 			1298
#define	FIN_PROGRAMA			1299
#define	EXCEPCION				1300
#define DESCONECTAR 			1301

#endif /* PROTOCOLO_H_ */
