/*
 * cpu.c
 *
 *  Created on: 29/4/2017
 *      Author: utnso
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/bitarray.h>
#include <commons/temporal.h>
#include <commons/process.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>

//Sigan las instrucciones de acá para instalar el parser acá:
//https://github.com/sisoputnfrba/ansisop-parser
#include <parser/metadata_program.h>

#include "../librerias/conexionSocket.h"
#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/controlErrores.h"
#include "../librerias/funcionesCPU.h"
#include "../librerias/pcb.h"
#include "../librerias/primitivasParser.h"
#include "../librerias/serializador.h"

// Ruta del archivo de configuración.
#define RUTA_ARCHIVO "./config_cpu.cfg"

// Operaciones que realiza la Memoria.
#define INICIAR_PROGRAMA 	51
#define SOLICITAR_BYTES_PAG 52
#define ALMACENAR_BYTES_PAG 53
#define ASIGNAR_PAGINAS_PRC 54
#define FINALIZAR_PRG 		55

int main(int argc, char *argv[]){

	// Variables importantes

	pcb PCB; 					// El registro PCB que usa la CPU para ejecutar instrucciones.

	u_int32_t id;				// Identificador del CPU en el Sistema Operativo.
	u_int32_t tamanioPaginas; 	// El tamaño de las páginas en Memoria. Se usa para calcular la página en la que se encuentra una instrucción.



	// Extracción de datos del archivo de configuración

	t_config* configuracion = llamarArchivo(RUTA_ARCHIVO);	// Lectura del archivo de configuración.

	char* ipKernel = busquedaClaveAlfanumerica(configuracion, "IP_KERNEL");		// IP del Kernel.
	char* ipMemoria = busquedaClaveAlfanumerica(configuracion, "IP_MEMORIA");	// IP de la Memoria.

	int puertoKernel = busquedaClaveNumerica(configuracion, "PUERTO_KERNEL");	// Puerto del Kernel,
	int puertoMemoria = busquedaClaveNumerica(configuracion, "PUERTO_MEMORIA");	// Puerto de la Memoria.



	// Conexión a servidores.

	int socketKernel = conectar(ipKernel, puertoKernel);
	// TODO: Handshake con Kernel.

	int socketMemoria = conectar(ipMemoria, puertoMemoria);
	// TODO: Handshake con Memoria.



	// Ciclo de instrucción del CPU.

	while (true)
	{
		// Espera a la recepción del PCB del Kernel para iniciar las operaciones.

		// TODO: Recibir PCB del Kernel.

		// Fetch stage

		solicitudMemoria peticion = generarSolicitudMemoria(&PCB, tamanioPaginas);
		char* instruccion = fetchInstruction(socketMemoria, peticion);

		// Execute stage

		/*
		Aca se debería inicializar las dos estructuras con todos los punteros a funciones (primitivas),
		que serían los dos punteros que recibe el analizador de linea

		llenarVarsPrimitivas(&PCB,&tamanioPaginas); //Sirve para que el .h disponga de las variables necesarias
		                                            //ya que no se pueden modificar los parametros de las primitivas

		char* instruccion;
		analizadorLinea(instruccion,);
		*/

		// Interrupciones
	}

	return 0;
}
