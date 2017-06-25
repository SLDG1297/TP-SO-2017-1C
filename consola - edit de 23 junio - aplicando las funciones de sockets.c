#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

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

#include "../librerias/controlArchivosDeConfiguracion.h"
#include "../librerias/controlErrores.h"

#define RUTA_ARCHIVO "./config_consola.cfg"
#define SIZE_DATA 1024
#define TRUE 1
#define FALSE 0

#include <semaphore.h>
#define TRUE 1
#define FALSE 0

 /* necesario para hilos de los datos ingresados por el usuario  */
int consolaEjecutando = TRUE;
char userInput[SIZE_DATA];
pthread_mutex_t lock= PTHREAD_MUTEX_INITIALIZER;
int ingresoDelUsuario=FALSE;
pthread_cond_t condicionLista; //para limitar el acceso a la lista




typedef struct formatoPrograma{
		char horaInicio [14];
		char horaFin [14];
		int programaPID;
		pthread_t threadID; /* aca guardo el ID del hilo creado con pthread_create */
		int cantImpresionesPantalla; /*contador */
		int programaEnEjecucion; //flag para saber cuándo debe finalizar

}unPrograma;  /* estructura que contiene los datos de un programa, esto estará en la lista*/



/*funciones creadas*/

void mostrarMenuEnPantalla();
int crearInfoPrograma();
void *ejecucionPrograma();
void *recibirUserInput();
void *analizarUserInput();
void calcularTiempoTotalEjecucion();


//Nombre de la lista de programas en ejecucion
t_list *listaProgramasActivos;


 /***********************
     EMPIEZA EL MAIN
 ***********************/

 int main(int argc, char *argv[]) {

//CODIGO PARA LLAMAR AL ARCHIVO

//Estructura para manejar el archivo de configuración -- t_config*
//Crear estructura de configuración para obtener los datos del archivo de configuración.

	t_config* configuracion;
	char* ruta = RUTA_ARCHIVO;
	// configuracion = llamarArchivo(ruta); 


//DECLARACION DE VARIABLES PARA EL CODIGO PRINCIPAL

	int bytesRecibidos,
		i,
		programID, //el Kernel devuelve el PID del programa al ser creado
		longitudBytesRecibidos;


	char datosEnviar[SIZE_DATA],
		 datosRecibir[SIZE_DATA],
		 idPrograma[SIZE_DATA],
		 pathPrograma[SIZE_DATA],
		 buffer[SIZE_DATA],
		 flagFinPrograma[SIZE_DATA]; //qué manda el kernel cuando se termina un programa -FALTA


	memset(datosEnviar, '\0', SIZE_DATA);
	memset(datosRecibir, '\0', SIZE_DATA);
	memset(idPrograma, '\0', SIZE_DATA);
	memset (buffer,'\0',SIZE_DATA);
	struct sockaddr_in kernel_dir;


//DECLARACION Y ASIGNACION DE DATOS PARA EL ARCHIVO DE CONFIGURACION

	//*Obtener IP del Kernel del archivo de configuración y chequear que sea correcto.
	char* IP_KERNEL = busquedaClaveAlfanumerica(configuracion, "IP_KERNEL");

	//*Obtener el puerto de Kernel del archivo de configuración y chequear que sea correcto.
	int PUERTO_KERNEL = busquedaClaveNumerica(configuracion, "PUERTO_KERNEL");


//DECLARACION DE VARIABLES PARA VALORES DE RESPUESTA

	int valorRtaConnect = 0;
	int valorRtaProgramaNuevo = 0;
	int valorRtaEnvioArchivo = 0;


//DECLARACION DE VARIABLES PARA EL MENU DEL USUARIO
char opcionIngresada;


//CREACION DE LA LISTA PARA GUARDAR LA INFO DE LOS PROGRAMAS
listaProgramasActivos = list_create(); //origen de la lista
pthread_cond_signal(&condicionLista); //semáforo para el acceso a la lista


// CODIGO PRINCIPAL DE LA CONSOLA
// host = gethostbyname(argv[1]);



//ASIGNACION DE DATOS DEL KERNEL

//*Direccion del servidor - KERNEL

/* kernel_DIR2 para destacar que lo cambie, por si hay error despues*/
//dirSock crearDireccion(char*ip, int puerto);
dirSock kernel_DIR2;
kernel_DIR2 = crearDireccion(IP_KERNEL, PUERTO_KERNEL);



//CONEXION A KERNEL
//creacion del socket y conexion
int sock = conectar(IP_KERNEL, PUERTO_KERNEL);
//todo- handshake


//*El cliente espera un mensaje de parte del kernel (handshake)

bytesRecibidos = recv(sock, datosRecibir, SIZE_DATA, 0);
datosRecibir[bytesRecibidos] = '\0';





//COMIENZO MENU USUARIO
    do
    { 	mostrarMenuEnPantalla();
	

		opcionIngresada = getchar(); //opcionIngresada del menu que ingresa el usuario
        switch(opcionIngresada)
        {
            case '1': /*CREAR PROGRAMA */

					puts("Ingrese la dirección del script AnSISOP\n");
					scanf("%s",pathPrograma);
					printf ("Path ingresado: %s \n  Iniciando programa. \n", pathPrograma);


					scanf("%s",datosEnviar);
					valorRtaEnvioArchivo= send(sock, datosEnviar, strlen(datosEnviar), 0);
					memset(datosEnviar, '\0', SIZE_DATA);
					esErrorSinSalida(valorRtaEnvioArchivo,"Error en Send");

					//recibir el PID del programa
					printf("el Program ID del programa es %d\n",programID);

					crearInfoPrograma(listaProgramasActivos,programID); //crea el hilo del programa

					


					memset (pathPrograma, '\0', SIZE_DATA);

			break;



			case '2': /*FINALIZAR PROGRAMA */

				if (list_is_empty(listaProgramasActivos) == 0){
						puts("No hay programas en ejecución \n");
				}
				else {
					//falta mostrar los programas abiertos?
					puts("Ingrese el PID del programa que desea finalizar:\n");

			


					char procesoACerrar[100]; //ver qué tamaño tiene el PID
				//	printf("Ingrese el número del programa a finalizar:\n");
					fgets(procesoACerrar,100,stdin);


					/*cerrar el hilo*/
					printf ("Finalizando programa %s \n", procesoACerrar);
					/*mostrar los tiempos de ejecución */
					printf ("Programa %s finalizado. \n", procesoACerrar);
				}

			break;


			case '3': /*DESCONECTAR CONSOLA */

					puts("Desconectando consola\n");

					// acceso a la lista 

					/* ITERAR LA LISTA DOS VECES:
					1- MOSTRAR LOS DATOS
					2- MANDAR A CANCELAR EL HILO  con pthread_cancel(); */


					list_clean(listaProgramasActivos);

					//finaliza conexion con el kernel
					cerrarSocket(sock);
					puts ("Consola desconectada. El menú finalizará. \n");

            break;


			case '4': /*LIMPIAR MENSAJES EN PANTALLA */

					system("clear");
					/*es un comando para la terminal de linux*/

			break;


			default: puts("Por favor, ingrese una opción válida.\n");

			break;
        }

    } while (opcionIngresada != '3');





	/*Recibir del Kernel el PID del proceso 	-FALTA */

		longitudBytesRecibidos = recv(i, buffer, sizeof(buffer), 0);
		if (longitudBytesRecibidos <= 0){
			perror("Error al recibir el PID del proceso");
			}
		memset(idPrograma, '\0', 1024);





	/* El kernel envía algo para imprimir por pantalla */
		longitudBytesRecibidos = recv(i, buffer, sizeof(buffer), 0);
		if (longitudBytesRecibidos <= 0){
			perror("Error al recibir el PID del proceso");
			}
		else if(buffer == flagFinPrograma){  /* Notificación de fin de programa */
		puts("Fin de programa\n");
		pthread_exit(0);
		}
		memset(idPrograma, '\0', 1024);



		//al final de t0do
		consolaEjecutando = FALSE;


	return 0;
}




/*
 *
 *
 *
 * END MAIN
 *
 *
 *
 *
 * */


//Añade un nodo a la lista e inicia un hilo
 int crearInfoPrograma (t_list *listaProgramasActivos, int programID) { /* devuelve 0 si esta t0do ok?*/

 		/*Asignacion inicial antes de crear el hilo */
 		unPrograma unThread;
 		strcpy(unThread.horaInicio,temporal_get_string_time());
 		unThread.cantImpresionesPantalla = 0;
 		unThread.programaEnEjecucion = TRUE;
		unThread.programaPID = programID;

		//mutex para leer posicion de la lista  
		pthread_mutex_lock(&lock);
		pthread_cond_wait(&condicionLista,&lock);

		//Añado el nodo a la lista con la info para el nuevo Hilo
 		int posicionEnLista = list_add(listaProgramasActivos,&unThread);


 		//Obtengo la info del nodo en la lista
 		/*es necesario para la funcion pthread_create */
 		unPrograma *datoHilo = list_get(listaProgramasActivos, posicionEnLista);

 		pthread_cond_signal(&condicionLista);
 		pthread_mutex_unlock(&lock);
 		//fin mutex para leer de la lista 

 		pthread_t *threadIDpointer;
 		threadIDpointer = &(datoHilo->threadID);


 		//Creacion del hilo
 		if (pthread_create(threadIDpointer,NULL,ejecucionPrograma,datoHilo) !=0)
 		{	//al programa le mando sus datos en el nodo
 		 	puts ("Error en la creacion del hilo del programa.\n");
 		}
 		
 		return 0;
 		}




//Linea de ejecucion para cada hilo
 void *ejecucionPrograma (unPrograma *datosDelPrograma) {

	 int sigueEjecutando = datosDelPrograma->programaEnEjecucion; //flag para saber que se está ejecutando

 	while(sigueEjecutando == TRUE){ /* mientras el programa ejecute */
 		char infoAMostrar[SIZE_DATA];

 		/*funcion para recibir lo que tengo que imprimir */

 		printf("El proceso %d informa: %s\n", datosDelPrograma->programaPID, infoAMostrar);
		datosDelPrograma->cantImpresionesPantalla ++;

		memset(infoAMostrar, '\0', SIZE_DATA);

 	}
/*Información a mostrar cuando termina un programa */

 		strcpy(datosDelPrograma->horaInicio,temporal_get_string_time());

 		//Calculo del tiempo total de ejecucion del programa
 		char tiempoTotalEjecucion [14];
 		calcularTiempoTotalEjecucion(datosDelPrograma->horaInicio,datosDelPrograma->horaFin,*tiempoTotalEjecucion);
 		// tiempoTotalEjecucion = datosDelPrograma -> horaInicio - datosDelPrograma -> horaFin  
 		


 		printf("Programa %d finalizado. Información de ejecución: \n",datosDelPrograma->programaPID);
 		printf("Hora de inicio de ejecución de %d: %p \n",datosDelPrograma->programaPID, datosDelPrograma->horaInicio);
 		printf("Hora de fin de ejecución de %d: %p \n",datosDelPrograma->programaPID, datosDelPrograma->horaFin);
 		//printf("Tiempo total de ejecucion de %d:%s \n", datosDelPrograma->programaPID, tiempoTotalEjecucion);    
 		printf("Cantidad de impresiones hechas en pantalla de %d:%d \n", datosDelPrograma->programaPID, datosDelPrograma->cantImpresionesPantalla);

/*El programa terminó su ejecución y libera el nodo */


 		//mutex modificar lista 
 		pthread_mutex_lock(&lock);
 		pthread_cond_wait(&condicionLista,&lock);

 		int posicionEnLista;
 		/* funcion para buscar cuál es de la lista*/
 //		list_find(listaProgramasActivos, bool(*closure)(void*)); 


 		list_remove_and_destroy_element(listaProgramasActivos, posicionEnLista, NULL);
 		//le puse null para que sea por defecto

 		pthread_cond_signal(&condicionLista);
 		pthread_mutex_unlock(&lock);
 		// mutex modificar lista

 	return 0;
 }


/*
 void * recibirUserInput()
		{
		        while(consolaEjecutando){
		                while(ingresoDelUsuario);
		                pthread_mutex_lock(&lock);
		                //printf("Enter a string: ");
		                scanf("%s",userInput);
		                ingresoDelUsuario=TRUE;
		                pthread_mutex_unlock(&lock);
		                pthread_cond_signal(&cond);
		        }
return 0;
}


void * analizarUserInput()
		{
		        while(consolaEjecutando == TRUE){
		                pthread_mutex_lock(&lock);
		                while(!ingresoDelUsuario)//se entra al loop sólo si el usuario ingresó info
		                        pthread_cond_wait(&cond,&lock);
		                      // printf("The string entered is %s\n",n);

		                        ingresoDelUsuario=FALSE; //se hizo lo que había que hacer con lo que ingresó el usuario
		                        pthread_mutex_unlock(&lock);
		        }
return 0;
		}

*/

void mostrarMenuEnPantalla(){
	puts("Ingrese una de las siguientes opciones:\n");
	puts("1- Iniciar programa.\n");
	puts("2- Finalizar un programa.\n");
	puts("3- Desconectar consola\n");
	puts("4- Limpiar mensajes\n");
}



//funcion para mostrar el tiempo, la estoy probando
void calcularTiempoTotalEjecucion(char horaInicio,char horaFin,char *tiempoTotalEjecucion){


}
